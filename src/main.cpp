#include "cam/OV2640.h"
#include "wifi_settings.h"
#include "constants.h"

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <WebSocketsServer.h>

// create wifi_settings.h and insert:
/*
#define myWifiSsid "yourSsid"
#define myWifiPassword "yourPassword"
*/
const char* ssid = myWifiSsid;
const char* password = myWifiPassword;

OV2640 cam;

WebServer webServer(80);
WebSocketsServer webSocketServer = WebSocketsServer(81);

// handling multiple clients
std::vector<WiFiClient> clientsVector;

// stream header
const char HEADER[] = "HTTP/1.1 200 OK\r\n" \
                      "Access-Control-Allow-Origin: *\r\n" \
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

// picture header
const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-disposition: inline; filename=capture.jpg\r\n" \
                       "Content-type: image/jpeg\r\n\r\n";
const int jhdLen = strlen(JHEADER);

void handle_new_streamClient() {
  //clients[clientsCounter] = webServer.client();
  WiFiClient wifiClient = webServer.client();
  wifiClient.write(HEADER, hdrLen);
  wifiClient.write(BOUNDARY, bdrLen);

  // set new client to vector
  clientsVector.push_back(wifiClient);
  //clientsCounter++;

  Serial.print("New one connected - ");
  Serial.println(wifiClient.remoteIP());
}

void handle_jpg() {
  WiFiClient client = webServer.client();

  if (!client.connected()) return;
  cam.makeFrameBuffer();
  client.write(JHEADER, jhdLen);
  client.write((char *)cam.getFrameBuffer(), cam.getSize());
}

void handle_test() {
  WiFiClient client = webServer.client();

  if (!client.connected()) return;
  Serial.println(webServer.arg("Test123")); //http://192.168.188.45/test?Test123=blablabla

}

// handle invalid webServer comands
void handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  webServer.send(200, "text / plain", message);
}

// handle incoming WS commands
void handle_WS(uint8_t num, uint8_t * payload) {
  // string to uint8_t -> "string".getBytes(); Maybe with string length
  // uint8_t to string -> (char *) payload

  String message = (char *)payload;
  Serial.println(message);
  if (message.indexOf(CAM_CONTROLS_PATH) != -1) {
    //camControls/brightness=1 -> -2 to 2
    String value = message.substring(message.indexOf("=") + 1, message.length());
    int newBrightness = atoi(value.c_str());
    cam.setBrightness(newBrightness);

    String txBrightness = CAM_CONTROLS_PATH + BRIGHTNESS_PATH + String(cam.getBrightness());
    webSocketServer.sendTXT(num, txBrightness);
  }
}

// is called when receiving any webSocket message
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // num = id of cliend, up to 5!
  // type = type of the message
  // payload = data as raw bytes
  // length = how many bytes are in the data

  // which webSocket event?
  switch(type) {
    // new client has connected
    case WStype_CONNECTED: {
        IPAddress ip = webSocketServer.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
        Serial.printf("Payload from %s; ", payload);
        Serial.printf("Type from %u; ", type);
        Serial.printf("Length from %u\n", length);

        // send camera settings here
        String txBrightness = CAM_CONTROLS_PATH + BRIGHTNESS_PATH + String(cam.getBrightness());
        Serial.println(txBrightness);
        webSocketServer.sendTXT(num, txBrightness);
      }
      break;

    // client has disconnected
    case WStype_DISCONNECTED: {
      IPAddress ip = webSocketServer.remoteIP(num);
      Serial.print(ip.toString());
      Serial.printf(" has disconnected!\n");

      }
      break;

    // echo text message back to client
    case WStype_TEXT: {
      Serial.printf("[%u] Text: %s\n", num, payload);
      handle_WS(num, payload);
    }
    break;

    // for everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

// handle disconnected clients and remove from clientsVector
void checkIfClientsDisconnect() {
  std::vector<int> disconnectedClients;
  for (std::vector<WiFiClient>::iterator i = clientsVector.begin(); i != clientsVector.end(); ++i) {
    // need the index to erase by the index
    if (!i.base()->connected()) {
      disconnectedClients.push_back(i - clientsVector.begin());
    }
  }

  for (int id : disconnectedClients) {
    // remove all clients by id
    clientsVector.erase(clientsVector.begin() + id);
  }
  disconnectedClients.clear();
}

// send frameBuffer to all clients
void handle_streams() {
  char buf[32];
  int s;

  cam.makeFrameBuffer();
  s = cam.getSize();

  for (WiFiClient client : clientsVector) {
      client.write(CTNTTYPE, cntLen);
      sprintf( buf, "%d\r\n\r\n", s );
      client.write(buf, strlen(buf));
      client.write((char *)cam.getFrameBuffer(), s);
      client.write(BOUNDARY, bdrLen);
  }
}

void setup() {
  Serial.begin(115200);
  //while (!Serial);            //wait for serial connection.

  cam.init();

  //WiFi.mode(WIFI_STA);
  WiFi.begin(myWifiSsid, myWifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  IPAddress ip = WiFi.localIP();
  Serial.print("WiFi connected - ip: ");
  Serial.println(ip);
  Serial.print("Stream Link: http://");
  Serial.print(ip);
  Serial.println(STREAM_PATH);
  Serial.print("Capture Link: http://");
  Serial.print(ip);
  Serial.println(CAPTURE_PATH);
  Serial.println("Test Send Link: http://192.168.188.45/test?Test123=blablabla");

  // setup webServer
  webServer.on(STREAM_PATH, HTTP_GET, handle_new_streamClient);
  webServer.on(CAPTURE_PATH, HTTP_GET, handle_jpg);
  webServer.on(TEST_PATH, HTTP_GET, handle_test); // https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
  webServer.onNotFound(handleNotFound);
  webServer.begin();

  // setup webSocketServer server
  webSocketServer.begin();
  webSocketServer.onEvent(onWebSocketEvent);
}

void loop() {
  // handle webServer data
  webServer.handleClient();

  // handle stream with all clients
  if (clientsVector.size() > 0) {
    handle_streams();
    checkIfClientsDisconnect();
  }

  // handle webSocket data
  webSocketServer.loop();
}
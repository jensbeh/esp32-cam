#include "cam/OV2640.h"
#include "wifi_settings.h"

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
WiFiClient clients[2];
int clientsCounter = 0;

const char HEADER[] = "HTTP/1.1 200 OK\r\n" \
                      "Access-Control-Allow-Origin: *\r\n" \
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n";
const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n";
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";
const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

void handle_jpg_stream() {
  char buf[32];
  int s;

  clients[clientsCounter] = webServer.client();

  clients[clientsCounter].write(HEADER, hdrLen);
  clients[clientsCounter].write(BOUNDARY, bdrLen);
  //clientsCounter++;

  while (true) {
    //for (int i = 0; i < (sizeof(clients)/sizeof(WiFiClient)); i++) {
        if (!clients[0].connected()) {
        //   clientsCounter--;
        //   clients[i] = NULL;
           break;
        }
        cam.makeFrameBuffer();
        s = cam.getSize();
        clients[0].write(CTNTTYPE, cntLen);
        sprintf( buf, "%d\r\n\r\n", s );
        clients[0].write(buf, strlen(buf));
        clients[0].write((char *)cam.getFrameBuffer(), s);
        clients[0].write(BOUNDARY, bdrLen);
    //}    

    webServer.handleClient();
    webSocketServer.loop();
  }
}

const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-disposition: inline; filename=capture.jpg\r\n" \
                       "Content-type: image/jpeg\r\n\r\n";
const int jhdLen = strlen(JHEADER);

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

void handle_WS(uint8_t num, uint8_t * payload) {
  // string to uint8_t -> "string".getBytes(); Maybe with string length
  // uint8_t to string -> (char *) payload

  String message = (char *)payload;
  webSocketServer.sendTXT(num, message);
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
      webSocketServer.sendTXT(num, payload);
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
  Serial.println("/stream");
  Serial.print("Capture Link: http://");
  Serial.print(ip);
  Serial.println("/capture");
  Serial.println("Test Send Link: http://192.168.188.45/test?Test123=blablabla");

  // setup webServer
  webServer.on("/stream", HTTP_GET, handle_jpg_stream);
  webServer.on("/capture", HTTP_GET, handle_jpg);
  webServer.on("/test", HTTP_GET, handle_test); // https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
  webServer.onNotFound(handleNotFound);
  webServer.begin();

  // setup webSocketServer server
  webSocketServer.begin();
  webSocketServer.onEvent(onWebSocketEvent);
}

void loop() {
  // handle webServer data
  webServer.handleClient();

  // handle webSocket data
  webSocketServer.loop();
}
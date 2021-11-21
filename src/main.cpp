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
std::vector<WiFiClient> wiFiClientsVector;
std::vector<uint8_t> clientIdsVector;

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
  wiFiClientsVector.push_back(wifiClient);
  //clientsCounter++;

  Serial.print("New WebServerClient connected - ");
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

void sendToWebSocketClients(String message) {
  for (uint8_t numId : clientIdsVector) {
    webSocketServer.sendTXT(numId, message);
  }
}

// handle incoming WS commands
void handle_WS(uint8_t num, uint8_t * payload) {
  // string to uint8_t -> "string".getBytes(); Maybe with string length
  // uint8_t to string -> (char *) payload

  String message = (char *)payload;
  Serial.println(message);
  // camControls/
  if (message.indexOf(CAM_CONTROLS_PATH) != -1) {

    //framesize= -> 0,3,4,5,6,7,8,9,10
    if (message.indexOf(FRAMESIZE_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int8_t newFramesize = atoi(value.c_str());
      cam.setFrameSize((framesize_t)newFramesize);

      sendToWebSocketClients(message);
    }

    //quality= -> 0 - 63
    else if (message.indexOf(QUALITY_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t newQuality = atoi(value.c_str());
      cam.setQuality(newQuality);

      sendToWebSocketClients(message);
    }

    //brightness= -> -2 to 2
    else if (message.indexOf(BRIGHTNESS_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int8_t newBrightness = atoi(value.c_str());
      cam.setBrightness(newBrightness);

      sendToWebSocketClients(message);
    }

    //contrast= -> -2 to 2
    else if (message.indexOf(CONTRAST_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int8_t newContrast = atoi(value.c_str());
      cam.setContrast(newContrast);

      sendToWebSocketClients(message);
    }

    //saturation= -> -2 to 2
    else if (message.indexOf(SATURATION_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int8_t newSaturation = atoi(value.c_str());
      cam.setSaturation(newSaturation);

      sendToWebSocketClients(message);
    }

    //specialEffect= -> 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
    else if (message.indexOf(SPECIAL_EFFECT_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t newSpecialEffect = atoi(value.c_str());
      cam.setSpecialEffect(newSpecialEffect);

      sendToWebSocketClients(message);
    }

    //whitebal= -> 0 = disable , 1 = enable
    else if (message.indexOf(WHITEBALANCE_STATE_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int whitebalState = atoi(value.c_str());
      cam.setAutoWhiteBalanceState(whitebalState);

      sendToWebSocketClients(message);
    }

    //awbGain= -> 0 = disable , 1 = enable
    else if (message.indexOf(AUTOWB_GAIN_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t autoWbGainState = atoi(value.c_str());
      cam.setAutoWbGainState(autoWbGainState);

      sendToWebSocketClients(message);
    }

    //wbMode= -> 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
    else if (message.indexOf(WB_MODE_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t newWbMode = atoi(value.c_str());
      cam.setWbMode(newWbMode);

      sendToWebSocketClients(message);
    }

    //exposureCtrl= -> 0 = disable , 1 = enable
    else if (message.indexOf(EXPOSURE_CTRL_STATE_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int exposureCtrlState = atoi(value.c_str());
      cam.setExposureCtrlState(exposureCtrlState);

      sendToWebSocketClients(message);
    }

    //aecValue= -> 0 - 1200
    else if (message.indexOf(AEC_VALUE_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint16_t newAecValue = atoi(value.c_str());
      cam.setAecValue(newAecValue);

      sendToWebSocketClients(message);
    }

    //aec2= -> 0 = disable , 1 = enable
    else if (message.indexOf(AEC2_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t aec2 = atoi(value.c_str());
      cam.setAec2(aec2);

      sendToWebSocketClients(message);
    }
    
    //aeLevel= -> -2 to 2
    else if (message.indexOf(AE_LEVEL_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int8_t newAeLevel = atoi(value.c_str());
      cam.setAeLevel(newAeLevel);

      sendToWebSocketClients(message);
    }

    //agc= -> 0 = disable , 1 = enable
    else if (message.indexOf(AGC_CTRL_STATE_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      int acgCtrlState = atoi(value.c_str());
      cam.setAgcCtrlState(acgCtrlState);

      sendToWebSocketClients(message);
    }

    //agcGain= -> 0 - 30
    else if (message.indexOf(AGC_GAIN_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t newAgcGain = atoi(value.c_str());
      cam.setAgcGain(newAgcGain);

      sendToWebSocketClients(message);
    }

    //gainceiling= -> 0 to 6
    else if (message.indexOf(GAINCEILING_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t newGainceiling = atoi(value.c_str());
      cam.setGainceiling(newGainceiling);

      sendToWebSocketClients(message);
    }

    //bpc= -> 0 = disable , 1 = enable
    else if (message.indexOf(BPC_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t bpc = atoi(value.c_str());
      cam.setBpc(bpc);

      sendToWebSocketClients(message);
    }

    //wpc= -> 0 = disable , 1 = enable
    else if (message.indexOf(WPC_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t wpc = atoi(value.c_str());
      cam.setWpc(wpc);

      sendToWebSocketClients(message);
    }

    //rawGma= -> 0 = disable , 1 = enable
    else if (message.indexOf(RAW_GMA_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t rawGma = atoi(value.c_str());
      cam.setRawGma(rawGma);

      sendToWebSocketClients(message);
    }

    //lenc= -> 0 = disable , 1 = enable
    else if (message.indexOf(LENC_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t lenc = atoi(value.c_str());
      cam.setLenc(lenc);

      sendToWebSocketClients(message);
    }
    
    //hmirror= -> 0 = disable , 1 = enable
    else if (message.indexOf(HMIRROR_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t hMirrorState = atoi(value.c_str());
      cam.setHmirrorState(hMirrorState);

      sendToWebSocketClients(message);
    }

    //vflip= -> 0 = disable , 1 = enable
    else if (message.indexOf(VFLIP_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t vFlipState = atoi(value.c_str());
      cam.setVflipState(vFlipState);

      sendToWebSocketClients(message);
    }

    //colorbar= -> 0 = disable , 1 = enable
    else if (message.indexOf(COLORBAR_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      uint8_t colorbar = atoi(value.c_str());
      cam.setColorbar(colorbar);

      sendToWebSocketClients(message);
    }
  }
}

// handle disconnected clients and remove from wiFiClientsVector
void removeWSClient(uint8_t num) {
  int index = 0;
  for (uint8_t numId : clientIdsVector) {
    if (numId == num) {
      break;
    } else {
      index++;
    }
  }

  // remove all clients by id
  clientIdsVector.erase(clientIdsVector.begin() + index);
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
      Serial.printf("[%u] New WSClient Connection from ", num);
      Serial.println(ip.toString());

      // add new clientId to clientIdsVector
      clientIdsVector.push_back(num);
      Serial.println("clientIdsVectorSize: " + String(clientIdsVector.size()));

      // send camera settings here
      // brightness
      String txBrightness = CAM_CONTROLS_PATH + BRIGHTNESS_PATH + String(cam.getBrightness());
      webSocketServer.sendTXT(num, txBrightness);
      // contrast
      String txContrast = CAM_CONTROLS_PATH + CONTRAST_PATH + String(cam.getContrast());
      webSocketServer.sendTXT(num, txContrast);
      }
      break;

    // client has disconnected
    case WStype_DISCONNECTED: {
      IPAddress ip = webSocketServer.remoteIP(num);

      // remove clientId from clientIdsVector
      removeWSClient(num);
      Serial.printf("WSClient has disconnected!\n");
      Serial.println("clientIdsVectorSize: " + String(clientIdsVector.size()));
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

// handle disconnected clients and remove from wiFiClientsVector
void checkIfClientsDisconnect() {
  std::vector<int> disconnectedClients;
  for (std::vector<WiFiClient>::iterator i = wiFiClientsVector.begin(); i != wiFiClientsVector.end(); ++i) {
    // need the index to erase by the index
    if (!i.base()->connected()) {
      disconnectedClients.push_back(i - wiFiClientsVector.begin());
    }
  }

  for (int id : disconnectedClients) {
    // remove all clients by id
    wiFiClientsVector.erase(wiFiClientsVector.begin() + id);
  }
  disconnectedClients.clear();
}

// send frameBuffer to all clients
void handle_streams() {
  char buf[32];
  int s;

  cam.makeFrameBuffer();
  s = cam.getSize();

  for (WiFiClient client : wiFiClientsVector) {
    // when writing and disconnect there is a error in log
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
  webServer.on(TEST_PATH, HTTP_GET, handle_test);
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
  if (wiFiClientsVector.size() > 0) {
    handle_streams();
    checkIfClientsDisconnect();
  }

  // handle webSocket data
  webSocketServer.loop();
}
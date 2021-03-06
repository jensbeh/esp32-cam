// WebServer old basics - Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD -> CameraWebServer Example with Apache License Version 2.0
// WebServer advanced -  https://github.com/arkhipenko/esp32-cam-mjpeg with BSD-3-Clause License
// WebSocketServer example - https://shawnhymel.com/1675/arduino-websocket-server-using-an-esp32/ 
// WebSocketLibrary https://github.com/Links2004/arduinoWebSockets - including LGPL-2.1 License
// Handle html - https://gist.github.com/GeorgeFlorian/7ce3de245e8a39434a5effc92d8f53a8
// Useful ESP wifi functions - https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
// reset file on SPIFFS - https://www.reddit.com/r/arduino/comments/idbyus/how_can_i_empty_a_text_file_from_my_sd_card/
// pinout from espCamera - https://randomnerdtutorials.com/esp32-cam-ai-thinker-pinout/

#include "main.h"
#include "cam/OV2640.h"
#include "constants.h"

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>

// variables
OV2640 cam;
uint8_t camFlashlightState = 0;
bool currentMotion = false;
std::vector<WiFiClient> wiFiClientsVector;
std::vector<uint8_t> clientIdsVector;

// server
WebServer webServer(80);
WebSocketsServer webSocketServer = WebSocketsServer(81);

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

// methods declarations
void sendCameraInitsToWebSocketClient(uint8_t num);
void resetWiFiCredentials();

/*
* method to handle/save new webServerClients to steam video to
*/
void handle_new_streamClient() {
  WiFiClient wifiClient = webServer.client();
  wifiClient.write(HEADER, hdrLen);
  wifiClient.write(BOUNDARY, bdrLen);

  // set new client to vector
  wiFiClientsVector.push_back(wifiClient);

  Serial.print("New WebServer-Client connected -> ");
  Serial.println(wifiClient.remoteIP());
}

/*
* method to handle invalid webServer commands
*/
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

/*
* method to send string message to all webSocketClients
*/
void sendToWebSocketClients(String message) {
  for (uint8_t numId : clientIdsVector) {
    webSocketServer.sendTXT(numId, message);
  }
}

/*
* method to send binary message to all webSocketClients like picture
*/
void sendToWebSocketClientsBin(uint8_t *payload, size_t length) {
  for (uint8_t numId : clientIdsVector) {
    webSocketServer.sendBIN(numId, payload, length);
  }
}

/*
* method to handle incoming webSocket commands
* sends message back to client as confirmation
*/
void handle_WS(uint8_t num, uint8_t * payload) {
  String message = (char *)payload;
  Serial.println(message);

  // camControls/
  if (message.indexOf(CAM_CONTROLS_PATH) != -1) {

    // websocketClient's espCamera needs to be updated
    if (message.indexOf(UPDATE_CAMERA_PATH) != -1) {
        sendCameraInitsToWebSocketClient(num);
    }
    // factory reset of esp
    else if (message.indexOf(FACTORY_RESET_ESP_PATH) != -1) {
        // reset esp camera values
        cam.resetValues();

        // reset esp wifi values
        resetWiFiCredentials();

        // restart esp
        ESP.restart();
    }
    // reset esp camera values to default
    else if (message.indexOf(RESET_CAM_VALUES_PATH) != -1) {
        // reset esp camera values and send all new values to all webSocketClients
        cam.resetValues();
        for (uint8_t numId : clientIdsVector) {
          sendCameraInitsToWebSocketClient(numId);
        }
    }

    //framesize= -> 0,3,4,5,6,7,8,9,10
    else if (message.indexOf(FRAMESIZE_PATH) != -1) {
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
    //flashlight= -> 0 = disable , 1 = enable
    else if (message.indexOf(FLASHLIGHT_PATH) != -1) {
      String value = message.substring(message.indexOf("=") + 1, message.length());
      camFlashlightState = atoi(value.c_str());
      if (camFlashlightState == 1) {
        digitalWrite(FLASHLIGHT_PIN, HIGH);
      } else {
        digitalWrite(FLASHLIGHT_PIN, LOW);
      }

      sendToWebSocketClients(message);
    }
  }

  cam.saveCameraSettings();
}

/*
* method to send the motion detected message and picture to all webSocketClients
*/
void sendMotionDetectionToClients() {
  // make picture
  cam.makeFrameBuffer();

  String message = CAM_NOTIFICATION_PATH + MOTION_DETECTED_PATH;
  sendToWebSocketClients(message);

  // send picture
  sendToWebSocketClientsBin(cam.getFrameBuffer(), cam.getSize());
}

/*
* method to handle disconnected webSocketClient and remove from clientIdsVector
*/
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

/*
* method to send all current values of the espCamera to the webSocketClient
*/
void sendCameraInitsToWebSocketClient(uint8_t num) {
        // framesize
      String txCmd = CAM_CONTROLS_PATH + FRAMESIZE_PATH + String(cam.getFrameSize());
      webSocketServer.sendTXT(num, txCmd);
      // quality
      txCmd = CAM_CONTROLS_PATH + QUALITY_PATH + String(cam.getQuality());
      webSocketServer.sendTXT(num, txCmd);
      // brightness
      txCmd = CAM_CONTROLS_PATH + BRIGHTNESS_PATH + String(cam.getBrightness());
      webSocketServer.sendTXT(num, txCmd);
      // contrast
      txCmd = CAM_CONTROLS_PATH + CONTRAST_PATH + String(cam.getContrast());
      webSocketServer.sendTXT(num, txCmd);
      // saturation
      txCmd = CAM_CONTROLS_PATH + SATURATION_PATH + String(cam.getSaturation());
      webSocketServer.sendTXT(num, txCmd);
      // special effect
      txCmd = CAM_CONTROLS_PATH + SPECIAL_EFFECT_PATH + String(cam.getSpecialEffect());
      webSocketServer.sendTXT(num, txCmd);
      // whitebalance state
      txCmd = CAM_CONTROLS_PATH + WHITEBALANCE_STATE_PATH + String(cam.getAutoWhiteBalanceState());
      webSocketServer.sendTXT(num, txCmd);
      // autoWbGain
      txCmd = CAM_CONTROLS_PATH + AUTOWB_GAIN_PATH + String(cam.getAwbGainSate());
      webSocketServer.sendTXT(num, txCmd);
      // wbMode
      txCmd = CAM_CONTROLS_PATH + WB_MODE_PATH + String(cam.getWbMode());
      webSocketServer.sendTXT(num, txCmd);
      // exposureCtrlState
      txCmd = CAM_CONTROLS_PATH + EXPOSURE_CTRL_STATE_PATH + String(cam.getExposureCtrlState());
      webSocketServer.sendTXT(num, txCmd);
      // aecValue
      txCmd = CAM_CONTROLS_PATH + AEC_VALUE_PATH + String(cam.getAecValue());
      webSocketServer.sendTXT(num, txCmd);
      // aec2
      txCmd = CAM_CONTROLS_PATH + AEC2_PATH + String(cam.getAec2());
      webSocketServer.sendTXT(num, txCmd);
      // aeLevel
      txCmd = CAM_CONTROLS_PATH + AE_LEVEL_PATH + String(cam.getAeLevel());
      webSocketServer.sendTXT(num, txCmd);
      // agcCtrlState
      txCmd = CAM_CONTROLS_PATH + AGC_CTRL_STATE_PATH + String(cam.getAgcCtrlState());
      webSocketServer.sendTXT(num, txCmd);
      // agcGain
      txCmd = CAM_CONTROLS_PATH + AGC_GAIN_PATH + String(cam.getAgcGain());
      webSocketServer.sendTXT(num, txCmd);
      // gainCeiling
      txCmd = CAM_CONTROLS_PATH + GAINCEILING_PATH + String(cam.getGainceiling());
      webSocketServer.sendTXT(num, txCmd);
      // bpc
      txCmd = CAM_CONTROLS_PATH + BPC_PATH + String(cam.getBpc());
      webSocketServer.sendTXT(num, txCmd);
      // wpc
      txCmd = CAM_CONTROLS_PATH + WPC_PATH + String(cam.getWpc());
      webSocketServer.sendTXT(num, txCmd);
      // wpc
      txCmd = CAM_CONTROLS_PATH + WPC_PATH + String(cam.getWpc());
      webSocketServer.sendTXT(num, txCmd);
      // rawGma
      txCmd = CAM_CONTROLS_PATH + RAW_GMA_PATH + String(cam.getRawGma());
      webSocketServer.sendTXT(num, txCmd);
      // lenc
      txCmd = CAM_CONTROLS_PATH + LENC_PATH + String(cam.getLenc());
      webSocketServer.sendTXT(num, txCmd);
      // hMirror
      txCmd = CAM_CONTROLS_PATH + HMIRROR_PATH + String(cam.getHmirror());
      webSocketServer.sendTXT(num, txCmd);
      // vFlip
      txCmd = CAM_CONTROLS_PATH + VFLIP_PATH + String(cam.getVflip());
      webSocketServer.sendTXT(num, txCmd);
      // colorbar
      txCmd = CAM_CONTROLS_PATH + COLORBAR_PATH + String(cam.getColorbar());
      webSocketServer.sendTXT(num, txCmd);
      // flashlight
      txCmd = CAM_CONTROLS_PATH + FLASHLIGHT_PATH + String(camFlashlightState);
      webSocketServer.sendTXT(num, txCmd);
}

/*
* method is called when receiving a webSocket Event (connected, disconnected, message, ...)
* num = id of cliend
* type = type of the event
* payload = data as raw bytes
* length = how many bytes are in the data
*/
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    // new client has connected
    case WStype_CONNECTED: {
      IPAddress ip = webSocketServer.remoteIP(num);
      Serial.printf("[%u] New WebSocketClient -> ", num);
      Serial.println(ip.toString());

      // add new clientId to clientIdsVector
      clientIdsVector.push_back(num);
      Serial.println("clientIdsVectorSize: " + String(clientIdsVector.size()));

      // send all camera settings here
      sendCameraInitsToWebSocketClient(num);
    }
    break;

    // client has disconnected
    case WStype_DISCONNECTED: {
      // remove clientId from clientIdsVector
      removeWSClient(num);
      Serial.printf("WSClient has disconnected!\n");
      Serial.println("clientIdsVectorSize: " + String(clientIdsVector.size()));
    }
    break;

    // handle incoming message
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

/*
* method to handle check if clients disconnected and remove from wiFiClientsVector for webServer-streams
* https://stackoverflow.com/questions/21341220/index-of-vector-iterator - to get the specific clientId from vector
*/
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

/*
* method to handle webServer camera stream
* sends frameBuffer to all clients
*/
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

/*
* method to create the main html-page with scanned and reachable wifi-networks
*/
void createIndexHtml() {
  // get all avaiable wifis nearby
  int n = WiFi.scanNetworks();

  // create newIndex/main html page
  String indexHtmlStr = indexHtml_part1;
  for (int i = 0; i < n; ++i) {
    String BSSIDstr = WiFi.BSSIDstr(i);
    BSSIDstr.replace(":", "");

    String htmlItem = indexHtml_partRawItem;
    htmlItem.replace("WIFI_ID", "xxx"+BSSIDstr+"xxx");
    htmlItem.replace("WIFI_NAME", WiFi.SSID(i));
    indexHtmlStr += htmlItem;

    if (i + 1 != n) {
      String htmlHrLine = indexHtml_hr_line;
      indexHtmlStr += htmlHrLine;
    }
  }
  indexHtmlStr += indexHtml_part2;

  // writes new index.html to storage of esp to send the new page to client
  File webFile = SPIFFS.open("/newIndex.html", "w");
  webFile.print(indexHtmlStr);
  webFile.close();
}

/*
* method to request for setup wifi with all avaiable wifi's it could connect to
*/
void handle_wifiSetupHtml() {
  // first create indexHtml with avaiable wifis
  createIndexHtml();

  // send indexHtml to client
  File webFile = SPIFFS.open("/newIndex.html", "r");
  webServer.send(200, "text/html", webFile.readString());
  webFile.close();
}

/*
* method to request the style.css file to client 
*/
void handle_wifiSetupCss() {
  // send style.css to client
  File webFile = SPIFFS.open("/style.css", "r");
  webServer.streamFile(webFile, "text/css");
  webFile.close();
}

/*
* method to request the jquery.js file to client 
*/
void handle_wifiSetupJQuery() {
  // send jquery.js to client
  File webFile = SPIFFS.open("/jquery.js", "r");
  webServer.streamFile(webFile, "text/js");
  webFile.close();
}

/*
* method to handle the "connect to wifi" action when user wants connects to wifi on index.html
* checks if the network can be connect -> send success back to client | else -> send error in credentials back to client
*/
void handle_incomingWifiCredentials() {
  if (webServer.hasArg("networkName") && webServer.hasArg("networkPassword")) {
    String name = webServer.arg("networkName").c_str();
    String pw = webServer.arg("networkPassword").c_str();
    const char *networkName = name.c_str();
    const char *networkPassword = pw.c_str();

    // check if password is correct else throw error
    WiFi.begin(networkName, networkPassword);
    delay(250);
    unsigned int startTime = millis();
    while ((WiFi.status() == WL_NO_SHIELD) || (WiFi.status() == WL_DISCONNECTED)) { // WL_NO_SHIELD = connecting to wifimodule; need to wait here with WL_DISCONNECTED because of wrong password
      delay(500);
      Serial.print(".");
      if (millis() - startTime >= 20000) { // timeout after 20 sec trying to connect
        break;
      }
    }
    // if wifi is connected
    if (WiFi.status() == WL_CONNECTED) {
      // write into SPIFFS
      File writeWifiCredentials = SPIFFS.open("/wifiCredentials.txt", "w");
      String toWrite = String(networkName) + '\n' + String(networkPassword) + '\n';
      writeWifiCredentials.println(toWrite);
      writeWifiCredentials.close();
      

      // send succes body - client will confirm if arrived with "showedSuccess=1" so the esp can be reset
      File webFile = SPIFFS.open("/success.html", "r");
      webServer.send(200, "text/html", webFile.readString());
      webFile.close();

    } else {
        WiFi.enableSTA(true); // needed because after "WL_DISCONNECTED" error e.g. wrong password the Station Mode (STA) is disabled???
        webServer.send(401, "text/plain", "Can't connect! Pls check your password...");
    }
  }

  else if (webServer.hasArg("showedSuccess")) {
      // restart when new credentials are set and successScreen was send to client - client will confirm with showedSuccess
      String state = webServer.arg("showedSuccess").c_str();
      if (state == "1") {
        ESP.restart();
      } 
  } else {
    webServer.send(401, "text/plain", "Wrong Args");
  }
}

/*
* method to reset espCameras wifi connection and restart the esp
*/
void resetWiFiCredentials() {
  SPIFFS.remove("/wifiCredentials.txt");

  File writeWifiCredentials = SPIFFS.open("/wifiCredentials.txt", "w");
  writeWifiCredentials.close();

  ESP.restart();
}

/*
* method to setup the esp on boot with all necessary stuff
*/
void setup() {
  Serial.begin(115200);

  // pins
  pinMode(FLASHLIGHT_PIN, OUTPUT);
  pinMode(MOTION_PIN, INPUT);     // declare sensor as input

  // read SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // get wifiCredentials if existing
  File readWifiCredentials = SPIFFS.open("/wifiCredentials.txt", "r"); 
  if(!readWifiCredentials){
      Serial.println("Failed to open file for reading");
      return;
  }
  char wifiCredentialsFileContent [512] = {'\0'};
  char wifiCredentialsSsid [512] = {'\0'};
  char wifiCredentialsPw [512] = {'\0'};
  uint16_t i = 0;
  while (readWifiCredentials.available()) {
     wifiCredentialsFileContent[i] = readWifiCredentials.read();
     i++;
  }
  readWifiCredentials.close();

  wifiCredentialsFileContent [i] ='\0';
  bool firstWord = true;
  bool secondWord = false;
  int lenFirstWord;
  for (int j = 0; j < i; j++) {
    if (wifiCredentialsFileContent[j] == '\n' && firstWord) {
      firstWord = false;
      secondWord = true;
      lenFirstWord = j + 1;
      continue;
    }
    else if (wifiCredentialsFileContent[j] == '\n' && secondWord) {
      secondWord = false;
      continue;
    }
    if (firstWord) {
      wifiCredentialsSsid[j] = wifiCredentialsFileContent[j];
    } else if (secondWord) {
      wifiCredentialsPw[j - lenFirstWord] = wifiCredentialsFileContent[j];
    }
  }

  Serial.println(wifiCredentialsSsid);
  Serial.println(wifiCredentialsPw);

  // if there are NO wifi credentials
  if (String(wifiCredentialsSsid) == "") {
    Serial.println("WiFi Credentials EMPTY");

    // name of the esp shown in wifi scanner
    const char *soft_ap_ssid = "ESP32 Camera";

    WiFi.mode(WIFI_AP_STA); // needs good quality power supply and cable and to not call the "Brownout detector was triggered"-error
    WiFi.softAP(soft_ap_ssid, NULL);
    Serial.printf("WiFi-SoftAP IP: -> ");
    Serial.println(WiFi.softAPIP());

    webServer.on("/", handle_wifiSetupHtml);
    webServer.on("/style.css", HTTP_GET, handle_wifiSetupCss);
    webServer.on("/jquery.js", HTTP_GET, handle_wifiSetupJQuery);
    webServer.on("/connectWiFi", handle_incomingWifiCredentials);
    webServer.onNotFound(handleNotFound);
    webServer.begin();
  } 
  // if there are wifi credentials
  else {
    Serial.println("WiFi Credentials NOT EMPTY");
    
    // setup cam
    cam.init();

    // setup wifi
    WiFi.mode(WIFI_STA); // needs good quality power supply and cable and to not call the "Brownout detector was triggered"-error
    WiFi.setHostname("ESP-Camera");
    WiFi.begin(wifiCredentialsSsid, wifiCredentialsPw);
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

    // setup webServer
    webServer.on(STREAM_PATH, HTTP_GET, handle_new_streamClient);
    webServer.on(RESET_WIFI_PATH, HTTP_GET, resetWiFiCredentials);
    webServer.onNotFound(handleNotFound);
    webServer.begin();

    // setup webSocketServer server
    webSocketServer.begin();
    webSocketServer.onEvent(onWebSocketEvent);
  }
}

/*
* method to loop the esp after setup
*/
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

  // motion detection logic
  int state = digitalRead(MOTION_PIN);
  if (state == HIGH && !currentMotion) {
    // motion begins
    currentMotion = true;
    Serial.println("MOTION START");

    // send motion webSocket message
    if (webSocketServer.connectedClients() > 0) {
      Serial.println("MOTION SEND TO CLIENTS");
      sendMotionDetectionToClients();
    }

  } else if (state == LOW && currentMotion) {
    // reset motion
    currentMotion = false;
    Serial.println("MOTION STOP");
  }
}
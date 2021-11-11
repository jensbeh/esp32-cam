#include "cam/OV2640.h"
#include "wifi_settings.h"

#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

// create wifi_settings.h and insert:
/*
#define myWifiSsid "yourSsid"
#define myWifiPassword "yourPassword"
*/
const char* ssid = myWifiSsid;
const char* password = myWifiPassword;

OV2640 cam;

WebServer server(80);

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

  clients[clientsCounter] = server.client();

  clients[clientsCounter].write(HEADER, hdrLen);
  clients[clientsCounter].write(BOUNDARY, bdrLen);
  clientsCounter++;

  while (true) {
    //for (int i = 0; i < (sizeof(clients)/sizeof(WiFiClient)); i++) {
        //if (!clients[i].connected()) {
        //   clientsCounter--;
        //   clients[i] = NULL;
        //   break;
        //}
        cam.makeFrameBuffer();
        s = cam.getSize();
        clients[0].write(CTNTTYPE, cntLen);
        sprintf( buf, "%d\r\n\r\n", s );
        clients[0].write(buf, strlen(buf));
        clients[0].write((char *)cam.getFrameBuffer(), s);
        clients[0].write(BOUNDARY, bdrLen);
    //}    

    server.handleClient();
  }
}

const char JHEADER[] = "HTTP/1.1 200 OK\r\n" \
                       "Content-disposition: inline; filename=capture.jpg\r\n" \
                       "Content-type: image/jpeg\r\n\r\n";
const int jhdLen = strlen(JHEADER);

void handle_jpg() {
  WiFiClient client = server.client();

  if (!client.connected()) return;
  cam.makeFrameBuffer();
  client.write(JHEADER, jhdLen);
  client.write((char *)cam.getFrameBuffer(), cam.getSize());
}

void handle_test() {
  WiFiClient client = server.client();

  if (!client.connected()) return;
  Serial.println(server.arg("Test123")); //http://192.168.188.45/test?Test123=blablabla

}

void handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text / plain", message);
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

  // server addresses
  server.on("/stream", HTTP_GET, handle_jpg_stream);
  server.on("/capture", HTTP_GET, handle_jpg);
  server.on("/test", HTTP_GET, handle_test); // https://techtutorialsx.com/2016/10/22/esp8266-webserver-getting-query-parameters/
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

const char* ssid     = "Smart-Soul";
const char* password = "86Irit86h,8iv'";


ESP8266WebServer server(80);

void fail(const char* msg) {
  Serial.println(msg);
}

void setup () {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  Serial.println("Waitting");
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
  WiFi.begin ( ssid, password );
  Serial.println ( "" );


  if (!SPIFFS.begin()) {
    fail("SPIFFS init failed");
  }
  Dir root = SPIFFS.openDir("/");
  Serial.println("READING ROOT");
  while (root.next()) {
    String fileName = root.fileName();
    File f = root.openFile("r");
    Serial.printf("%s: %d\r\n", fileName.c_str(), f.size());
  }
  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.print("WIFI CONNECTED => ");
  Serial.println(WiFi.localIP());
  server.on ( "/", [](){
    server.send(200, "text/plain", "this root path");
  });
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void loop() {
    server.handleClient();
}

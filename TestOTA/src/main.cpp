#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define D1 5
#define D2 4
#define ledPin_right  D1
#define ledPin_left   D2

const char* ssid     = "Smart-Soul";
const char* password = "86Irit86h,8iv'";

//const char* host = "OTA-testing";
//const char* ota_pass = "ota";

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) // check connection with router
  {
    Serial.println("Connection Router Failed! Rebooting...");
    delay(500);
    ESP.restart();
  }

  //ArduinoOTA.setPort(8266); // Port defaults to 8266
  //ArduinoOTA.setHostname(host); // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setPassword(ota_pass); // No authentication by default

  ArduinoOTA.onStart([]()
  {
   Serial.println("Start");
  });
  ArduinoOTA.onEnd([]()
  {
    Serial.println("End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    ESP.restart();
  });

  ArduinoOTA.begin(); // setup the OTA server
  Serial.println("Ready!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(ledPin_right,OUTPUT);
  pinMode(ledPin_left,OUTPUT);

  digitalWrite(ledPin_right,LOW);
  digitalWrite(ledPin_left,LOW);

}

void loop()
{
  ArduinoOTA.handle();

  digitalWrite(ledPin_right,!digitalRead(ledPin_right));
  //digitalWrite(ledPin_left,!digitalRead(ledPin_left));
  delay(250);
}

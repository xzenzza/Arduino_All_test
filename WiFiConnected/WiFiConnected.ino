// Copyright Nat Weerawan 2015-2016
// MIT License

#include <ESP8266WiFi.h>
#include <WiFiConnector.h>

WiFiConnector *wifi;

#define WIFI_SSID        "cccc"
#define WIFI_PASSPHARSE  "cccc"
uint8_t SMARTCONFIG_BUTTON_PIN = 0;

#include "init_wifi.h"

void init_hardware()
{
  Serial.begin(115200);
  delay(1000);
  Serial.flush();
  Serial.println();
  Serial.println();
  Serial.println("will be started in 500ms..");
}

void setup()
{
  WiFi.enableAP(false);
  init_hardware();
  delay(200);
  wifi = init_wifi(WIFI_SSID, WIFI_PASSPHARSE, SMARTCONFIG_BUTTON_PIN);
  Serial.println("BEING CONNECTED TO: ");
  Serial.println(String(wifi->SSID() + ", " + wifi->psk()));

  wifi->on_connecting([&](const void* message)
  {
    Serial.print("STATE: CONNECTING -> ");
    Serial.println(wifi->counter);
    delay(500);
  });

  wifi->on_connected([&](const void* message)
  {
    // Print the IP address
    Serial.print("WIFI CONNECTED WITH IP: ");
    Serial.println(WiFi.localIP());
  });

  wifi->connect();

}

void loop()
{
  wifi->loop();
}


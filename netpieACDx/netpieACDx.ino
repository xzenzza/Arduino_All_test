#include <AuthClient.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <SHA1.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <MicroGear.h>

const char* ssid     = "Smart-Soul";
const char* password = "86Irit86h,8iv'";

#define APPID       "application01"
#define GEARKEY     "pKLfSe5nuWlENSc"
#define GEARSECRET  "mHo39YqqL326yoaBuWTZoZVStJ6zJz"
#define SCOPE       ""

int ADC_Value = 0;

WiFiClient client;
AuthClient *authclient;

int timer = 0;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen)
{
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen)
{
    Serial.print("Found new member --> ");
    for (int i=0; i<msglen; i++)
    {
        Serial.print((char)msg[i]);
    }
    Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen)
{
    Serial.print("Lost member --> ");
    for (int i=0; i<msglen; i++)
    {
      Serial.print((char)msg[i]);
    }
    Serial.println();
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen)
{
    Serial.println("Connected to NETPIE...");
    Serial.println(ADC_Value);
    microgear.setName("ADC_Value");
}


void setup()
{
    /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(PRESENT,onFoundgear);
    microgear.on(ABSENT,onLostgear);
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
       delay(250);
       Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    //uncomment the line below if you want to reset token -->
    microgear.resetToken();
    microgear.init(GEARKEY,GEARSECRET,SCOPE);
    microgear.connect(APPID);

}

void loop()
{
    if (microgear.connected())
    {
        microgear.loop();
        timer = 0;
        Serial.println("connected");

        microgear.loop();
        ADC_Value = analogRead(A0);

        char msg[128];
        sprintf(msg, "%d,,,ADC_Value,Value of ADC", ADC_Value);

        microgear.chat("webapp", msg);
    }
    else
    {
        Serial.println("connection lost, reconnect...");
        microgear.connect(APPID);
        delay(timer);
        timer += 500;
    }
    delay(1000);
}

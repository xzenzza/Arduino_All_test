/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */
 // Example testing sketch for various DHT humidity
//temperature sensors
// Written by ladyada, public domain
// DHT_dual_test
// Demonstrates multiple sensors
// Modified sketch by DIY-SciB.org
#include "DHT.h"
#define DHT1PIN 2
// what pin we're connected to
#define DHT2PIN 3
// Uncomment whatever type you're using!
//#define DHT1TYPE DHT11 // DHT 11
#define DHT2TYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)
// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor
DHT dht1(DHT1PIN, DHT2TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);
#define Solenoid_1 13
#define Solenoid_2 12


#include <ESP8266WiFi.h>

const char* ssid     = "Smart-Soul";
const char* password = "86Irit86h,8iv'";

const char* host = "https://api.thingspeak.com";

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  dht1.begin();
  dht2.begin();

  pinMode(Solenoid_1,OUTPUT);
  pinMode(Solenoid_2,OUTPUT);
  digitalWrite(Solenoid_1, LOW);
  digitalWrite(Solenoid_2, LOW);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
/*
IPAddress local_ip = {192,168,1,22};   //ตั้งค่า IP
IPAddress gateway={192,168,1,1};    //ตั้งค่า IP Gateway
IPAddress subnet={255,255,255,0};   //ตั้งค่า Subnet
WiFi.config(local_ip,gateway,subnet);   //setค่าไปยังโมดูล
*/
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void On_status()
{
  digitalWrite(Solenoid_1,HIGH);
  digitalWrite(Solenoid_2,HIGH);
  delay(60000);
  digitalWrite(Solenoid_1,LOW);
  digitalWrite(Solenoid_2,LOW);
  delay(10000);
}


void loop() {
  delay(2000);

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h1 = dht1.readHumidity();
  float t1 = dht1.readTemperature();
  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
/*  if (isnan(t1) || isnan(h1))
    {
      Serial.println("Failed to read from DHT #1");
    }
  else
  { */
    Serial.print("Humidity 1: ");
    Serial.print(h1); Serial.print(" %\t");
    Serial.print("Temperature 1: ");
    Serial.print(t1); Serial.println(" *C");

    int hum1 = h1;
    int temp1 = t1;
    if( hum1 <= 20.00 || temp1 >= 30.00)
      {
        //digitalWrite(Solenoid_1, HIGH);
       On_status();
      }
      else
      {
        digitalWrite(Solenoid_1, LOW);
        digitalWrite(Solenoid_2, LOW);
      }
  //  }
 // if (isnan(t2) || isnan(h2))
 //   {
 //     Serial.println("Failed to read from DHT #2");
 //   }
//  else
 // {
    Serial.print("Humidity 2: ");
    Serial.print(h2); Serial.print(" %\t");
    Serial.print("Temperature 2: ");
    Serial.print(t2); Serial.println(" *C");

    int hum2 = h2;
    int temp2 = t2;
    if(hum2 <= 20.00 || temp2 >= 35.00)
      {
      //
     On_status();
      //digitalWrite(Solenoid_2, HIGH);
      }
      else
      {
        digitalWrite(Solenoid_1, LOW);
        digitalWrite(Solenoid_2, LOW);
      }

//  }
    Serial.println();
    delay(100);


  Serial.print("Requesting URL: ");
//  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET /update?key=84XWXY1ODD3G51TO&field1=" + String(t1) + "&field2=" + String(h1) + "&field3=" + String(t2) + "&field4=" +String(h2) )+ " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  /*
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  */

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}

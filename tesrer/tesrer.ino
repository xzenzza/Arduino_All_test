#include <Arduino.h>
#include "DHT.h"
#include "CMMCInterval.h"
#include <ESP8266WiFi.h>
//#include <ArduinoJSON.h>

#include <ESP8266HTTPClient.h>

//const char* ssid = "Smart-Soul";
//const char* password = "86Irit86h,8iv'";
const char* ssid = "@RMUTI-WiFi";
const char* password = "";


#define DHT1PIN D1
#define DHT2PIN D2
#define Solenoid_1 D0 //Temperature
#define Solenoid_2 D3  //Humidity

float h1,h2,t1,t2;
// Uncomment whatever type you're using!
//#define DHT1TYPE DHT11 // DHT 11
#define DHT2TYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht1(DHT1PIN, DHT2TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);

//const char* ssid     = "Smart-Soul";
//const char* password = "86Irit86h,8iv'";                             //const char* ssid     = "AP_Baanphaon";
                                                                    //const char* password = "012346789";

const char* host = "api.thingspeak.com";

    int hum1 = h1;
    int temp1 = t1;
    int hum2 = h2;
    int temp2 = t2;

void Control_Function();
void Update_Thingspeak();
void GetJson_Thingspeak();

CMMCInterval myInterval1(1000, Control_Function);
CMMCInterval myInterval2(1000, Update_Thingspeak);
CMMCInterval myInterval3(1000, GetJson_Thingspeak);

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

 //----------------------------------Authen--------------------------///
 HTTPClient http;
       Serial.println("[HTTP] begin...\n");
        http.begin("http://afw-kkc.rmuti.ac.th/login.php"); //HTTP
        Serial.println("[HTTP] POST...\n");
        int httpCode = http.POST("_u=theerasak.su&_p=t040350579&a=a&web_host=afw-kkc.rmuti.ac.th&web_host4=afw4-kkc.rmuti.ac.th&_ip4=172.26.110.148&web_host6=&_ip6=");
        if(httpCode > 0) {
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void On_Temp(){  //On_temp
  digitalWrite(Solenoid_1,HIGH);
  delay(60000);
  digitalWrite(Solenoid_1,LOW);
  delay(10000);
  Serial.println("On_Temp");
}
void On_Humid(){  //On_Hum
  digitalWrite(Solenoid_2,HIGH);
  delay(60000);
  digitalWrite(Solenoid_2,LOW);
  delay(10000);
  Serial.println("On_Humid");
}
void Off_Temp(){
  digitalWrite(Solenoid_1,LOW);
  delay(30000);
  Serial.println("Off_Temp");
}
void Off_Humid(){
  digitalWrite(Solenoid_2,LOW);
  delay(30000);
  Serial.println("Off_Humid");

}
void Update_Thingspeak(){

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
     Serial.println();
    delay(100);

  Serial.print("Requesting URL: ");
//  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET /update?key=VVZRCHVPSD3SF2NF&field1=" + String(t1) + "&field2=" + String(h1) + "&field3=" + String(t2) + "&field4=" +String(h2) )+ " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");


  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }


  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

}

void GetJson_Thingspeak(){

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
     Serial.println();
    delay(100);

  Serial.print("Requesting URL: ");
//  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET /channels/97599/feed/last" )+ " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");


  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }


  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

}
void Control_Function(){
    h1 = dht1.readHumidity();
    t1 = dht1.readTemperature();
    h2 = dht2.readHumidity();
    t2 = dht2.readTemperature();

if (isnan(t1) || isnan(t2)) {
  Serial.println("Failed to read from DHT Temperature");
  t1 = 0.00;
  t2 = 0.00;
  digitalWrite(Solenoid_1,LOW);
  }
  else
  {
    Serial.print("Humidity 1: ");
    Serial.print(h1); Serial.print(" %\t");
    Serial.print("Temperature 1: ");
    Serial.print(t1); Serial.println(" *C");



    if( temp1 >= 35.00 || temp2 >= 35.00){
         On_Temp();
        }
    if( temp1 <= 28.00 || temp2 <= 28.00){
          Off_Temp();
        }
    }
if (isnan(h1) || isnan(h2)) {
      h1 = 100.00;
      h2 = 100.00;
      Serial.println("Failed to read from DHT #Humidity");
      digitalWrite(Solenoid_2,LOW);
  }
  else
  {
    Serial.print("Humidity 2: ");
    Serial.print(h2); Serial.print(" %\t");
    Serial.print("Temperature 2: ");
    Serial.print(t2); Serial.println(" *C");


    if(hum1 <= 30.00 || hum2 <= 30.00){
      On_Humid();
      }
    if(hum1 >= 45.00 || hum2 >= 45.00){
      Off_Humid();
      }
   }
}

void loop() {

  myInterval1.Update();
  myInterval2.Update();
  myInterval3.Update();
}


#include <ESP8266WiFi.h>

#include "EmonLib.h"                   // Include Emon Library
EnergyMonitor emon1;                   // Create an instance

double Irms; 

const char* ssid     = "C&amp;amp;EST_RMUTI"; 
//const char* ssid     = "Internet_of_Farm";  
const char* password = "123467890";
const char* host = "api.thingspeak.com";
void setup() {
 
 Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  //---------------------Connet_Router--------------------//
  Serial.println(ssid);
  WiFi.begin(ssid, password);
emon1.current(A0,31.0303);             // Current: input pin, calibration.
  //-----------------------------------------------------//
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  //--------------------------Conneting----------------------//
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   

}
//---------------------------------------------------------//

void Tell_Status_Work(){
  
  digitalWrite(BUILTIN_LED,LOW); delay(5);
  digitalWrite(BUILTIN_LED,HIGH); delay(10);
}

//---------------------------------------------------------//

void loop() {
  
  Tell_Status_Work();
Irms = emon1.calcIrms(1480);  // Calculate Irms only

  Serial.print(" A = ");
 Serial.println(Irms);          // Irms

  delay(2000);
  Serial.print("connecting to ");
  Serial.println(host);
  
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }                                                     

  /*--------------------------------------------------------------------V_Fan-------------------I_Fan-------------------I_Com--------------------Noise--------------------t1----------------------h1----------------------t2----------------------h2-------*/
  
  client.print(String("GET /update?key=BK0EFRABG7ZYA502&field3=" + String(Irms)) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);
  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
}

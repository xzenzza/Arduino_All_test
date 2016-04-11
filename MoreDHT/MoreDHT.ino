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

void setup() 
{ 
  Serial.begin(9600); 
  Serial.println("DHTxx test!");   
  dht1.begin();
  dht2.begin(); 

  pinMode(Solenoid_1,OUTPUT);
  pinMode(Solenoid_2,OUTPUT);
  digitalWrite(Solenoid_1, LOW);
  digitalWrite(Solenoid_2, LOW);
}   

void loop() 
{ 
// Reading temperature or humidity takes about 250 milliseconds! 
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor) 
  float h1 = dht1.readHumidity(); 
  float t1 = dht1.readTemperature(); 
  float h2 = dht2.readHumidity(); 
  float t2 = dht2.readTemperature();   
  // check if returns are valid, if they are NaN (not a number) then something went wrong! 
  if (isnan(t1) || isnan(h1)) 
  { 
    Serial.println("Failed to read from DHT #1"); 
  } 
  else 
  { 
    Serial.print("Humidity 1: "); 
    Serial.print(h1); Serial.print(" %\t");
    Serial.print("Temperature 1: "); 
    Serial.print(t1); Serial.println(" *C"); 
    
    int hum1 = h1;
    int temp1 = t1;
    if( hum1 <= 80.00 && temp1 >= 35.00)
      {
        digitalWrite(Solenoid_1, HIGH); 
      }
      else digitalWrite(Solenoid_1, LOW); 
  } 
  if (isnan(t2) || isnan(h2)) 
  { 
    Serial.println("Failed to read from DHT #2"); 
  } 
  else 
  { 
    Serial.print("Humidity 2: "); 
    Serial.print(h2); Serial.print(" %\t"); 
    Serial.print("Temperature 2: "); 
    Serial.print(t2); Serial.println(" *C");

    int hum2 = h2;
    int temp2 = t2;
    if(hum2 <= 80.00 && temp2 >= 35.00)
      {
        digitalWrite(Solenoid_2, HIGH); 
      } 
      else digitalWrite(Solenoid_2, LOW);
      
  } 
    Serial.println(); 
    delay(1000);
} 
  //- See more at: http://diy-scib.org/blog/multiple-dht-humidity-sensors-arduino#sthash.TWvPv7qH.dpuf

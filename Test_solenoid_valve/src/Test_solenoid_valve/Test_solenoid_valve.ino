#include <Arduino.h>

  unsigned int Sec = 1000;
  unsigned int Min = 60;
void setup() {
  // put your setup code here, to run once:
pinMode(D2,OUTPUT);
pinMode(D3,OUTPUT);
digitalWrite(D2,LOW);
digitalWrite(D3,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(D2,HIGH);
digitalWrite(D3,LOW);
delay(10000);
digitalWrite(D3,HIGH);
digitalWrite(D2,LOW);
delay(10000);
digitalWrite(D2,HIGH);
digitalWrite(D3,HIGH);
delay(5000);



}

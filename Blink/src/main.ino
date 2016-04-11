#include <Arduino.h>
#define LED1 1
#define LED2 13
  unsigned int Sec = 1000;
  unsigned int Min = 60;
void setup() {
  // put your setup code here, to run once:
pinMode(LED1,OUTPUT);
pinMode(LED2,OUTPUT);
digitalWrite(LED1,LOW);
digitalWrite(LED2,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(LED1,HIGH);
digitalWrite(LED2,LOW);
delay(1000);
digitalWrite(LED2,HIGH);
digitalWrite(LED1,LOW);
delay(1000);
digitalWrite(LED1,HIGH);
digitalWrite(LED2,HIGH);
delay(5000);

}



#include <ESP8266WiFi.h>

#define A_Pin 12
#define B_Pin 13
#define C_Pin 14
#define D_Pin 15
#define LED_Pin 4 

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);

pinMode(A_Pin,OUTPUT);
pinMode(B_Pin,OUTPUT);
pinMode(C_Pin,OUTPUT);
pinMode(D_Pin,OUTPUT);
pinMode(LED_Pin,OUTPUT);

}

void PortA_Read(){
digitalWrite(A_Pin,HIGH);
digitalWrite(B_Pin,LOW);
digitalWrite(C_Pin,LOW);
digitalWrite(D_Pin,LOW);
delay(1000);
Serial.print("Port A Value : \t");
Serial.println(analogRead(A0));
delay(1000);

}

void PortB_Read(){
digitalWrite(A_Pin,LOW);
digitalWrite(B_Pin,HIGH);
digitalWrite(C_Pin,LOW);
digitalWrite(D_Pin,LOW);
delay(1000);
Serial.print("Port B Value : \t");
Serial.println(analogRead(A0));
delay(1000);

}
void PortC_Read(){

digitalWrite(A_Pin,LOW);
digitalWrite(B_Pin,LOW);
digitalWrite(C_Pin,HIGH);
digitalWrite(D_Pin,LOW);
delay(1000);
Serial.print("Port C Value : \t");
Serial.println(analogRead(A0));
delay(1000);

}
void PortD_Read(){

digitalWrite(A_Pin,LOW);
digitalWrite(B_Pin,LOW);
digitalWrite(C_Pin,LOW);
digitalWrite(D_Pin,HIGH);
delay(1000);
Serial.print("Port D Value : \t");
Serial.println(analogRead(A0));
delay(1000);
}
void LED_Blink(){
	digitalWrite(LED_Pin,HIGH);delay(100);
	digitalWrite(LED_Pin,LOW);delay(100);
}
void loop() {
  // put your main code here, to run repeatedly:
//PortA_Read(); //volt
PortB_Read();  //acs712
//PortC_Read();  //CT
//PortD_Read(); // mic
delay(100);
LED_Blink();
Serial.println("Test Completed");
}

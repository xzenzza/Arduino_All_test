void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(12,OUTPUT);
pinMode(13,OUTPUT);
pinMode(14,OUTPUT);
pinMode(15,OUTPUT);
digitalWrite(12,LOW);
digitalWrite(13,HIGH);
digitalWrite(14,LOW);
digitalWrite(15,LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
Serial.println(analogRead(A0));
delay(100);
}

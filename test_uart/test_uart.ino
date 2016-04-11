String num;
void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
Serial.println("Setting");
 delay(300);
}

void loop() {
  if(Serial.available()>0){
    num = Serial.read();
    Serial.println(num);
    
  }

}

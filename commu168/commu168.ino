void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  String inByte;

  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();

    Serial.println(inByte);
  }
  

}

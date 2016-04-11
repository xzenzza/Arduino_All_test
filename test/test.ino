void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}
float I_COM = 0;
void loop() {
  // put your main code here, to run repeatedly:
  I_COM = analogRead(A0);
  I_COM = ((I_COM)*(0.01897912986));
  Serial.print(" A = ");
  Serial.println(I_COM);
  delay(3000);
}

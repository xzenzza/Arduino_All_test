#define pinButton_01 2
#define pinButton_02 3
#define LED_01 12
#define LED_02 13
int stateLED_01 = LOW;
int stateLED_02 = LOW;
int stateButton_01,stateButton_02;
int previous_01 = LOW;
int previous_02 = LOW;
long time = 0;
long debounce = 200;
 
void setup() {
  pinMode(pinButton_01, INPUT);
  pinMode(pinButton_02, INPUT);
  pinMode(LED_01, OUTPUT);
  pinMode(LED_02, OUTPUT);
}
 
void loop() {
  stateButton_01 = digitalRead(pinButton_01);  
  if(stateButton_01 == HIGH && previous_01 == LOW && millis() - time > debounce) {
    if(stateLED_01 == HIGH){
      stateLED_01 = LOW; 
    } else {
       stateLED_01 = HIGH; 
    }
    time = millis();
    delay(50);
  }
  digitalWrite(LED_01, stateLED_01);
  previous_01 == stateButton_01;

  stateButton_02 = digitalRead(pinButton_02);  
  if(stateButton_02 == HIGH && previous_02 == LOW && millis() - time > debounce) {
    if(stateLED_02 == HIGH){
      stateLED_02 = LOW; 
    } else {
       stateLED_02 = HIGH; 
    }
    time = millis();
    delay(50);
  }
  digitalWrite(LED_02, stateLED_02);
  previous_02 == stateButton_02;
}
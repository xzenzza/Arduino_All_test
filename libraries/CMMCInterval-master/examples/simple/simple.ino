#include "CMMCInterval.h"

void HelloWorld();
void LoveYou();

CMMCInterval myInterval1(1000, HelloWorld);
CMMCInterval myInterval2(500, LoveYou);

void setup() {
  Serial.begin(9600);
}

void loop() {
  myInterval1.Update();
  myInterval2.Update();
}

void HelloWorld()
{
    Serial.println("Helloworld");
}

void LoveYou()
{
    Serial.println("LoveYou");
}




/*
This example demonstrate the usage of the A/D Converter with MCP3551 library.
A good explanation on the code can be found in application note AN1154 by MCP.

Copyright (c) 2013 Simon Bauer.  All rights reserved.

This example code is in the public domain.
*/
#include <MCP3551.h>
#include <SPI.h>


// digital pin 2 has a pushbutton attached to it. Give it a name:
const int pushButton = 2;

// Slave select pin:
const int MCPPin = 30;

//Some variables we need:
float calRAdevice1 = 13607; //stores the series resistor value 
float RTD; //resistor of RTD
const int RZero = 100; //resistance at 0°C

//create a instance with IC connect to SCK, MOSI and SS (MCPPin):
MCP3551 myRTD(MCPPin);

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(pushButton, INPUT);

}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  int buttonState = digitalRead(pushButton);

  bool rtdReady = myRTD.getCode();
  if(rtdReady)
  {
    // Serial.println(getRTD1.byteCode);
    
    //calculate RTD acc. MCP AN1154:
    RTD = calRAdevice1 * (float(myRTD.byteCode) / ( 2097152.0 - float(myRTD.byteCode)));
    
    //This is part of a calculation for T(RTD)
    RTD = (RTD / RZero) - 1;
    float temperature = (RTD * (255.8723 + RTD * (9.6 + RTD * 0.878)));
    
    Serial.print("Temperature: "); Serial.println(temperature);
  }

  // A simple method for calibration at 0°C with iced water:
  if (buttonState)
  {
    char n=0;
    float temp=0;
    do 
    {
      if (myRTD.getCode());
      {
        temp+= float(myRTD.byteCode);
        n++;  
      }
      
    } while (n<10);
  
    temp /= 10; 
    calRAdevice1 = RZero;
    calRAdevice1 /= ( temp /( 2097152.0 - temp));
    
    delay(2000);
    Serial.print("Ra: "); Serial.println(calRAdevice1);
    delay(2000);
      }

}

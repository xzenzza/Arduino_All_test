/******************************************************************************************************************************************************
 * Arduino MCP4812 library - Version 0.1
 *
 * Copyright (c) 2013 Simon Bauer.  All rights reserved.
 *
 * This library implements the SPI communication to one or more MCP4812 DAC devices.
 *
 *
 * This code is licensed under a GPLv3 License.
 *
 *
 **********************************************************************************************/
#include "MCP4812/MCP4812.h"

MCP4812::MCP4812 (uint8_t devicePin, uint8_t myGain)
{
	myDevice = devicePin;
	gain = myGain;
	pinMode(myDevice,OUTPUT);
	digitalWrite(myDevice,HIGH);
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV16);
}

boolean MCP4812::write(uint8_t channel , uint16_t dacValue)
{
	
	if (dacValue <= 1023 && channel <= 1) //higher values corrupte data. 
	{
		volatile union{
			uint16_t transferInt;
			uint8_t aByte[2];
		}c;
		active_mode=1;
		c.transferInt = dacValue;
		c.transferInt = c.transferInt << 2; //as we have only 10bit with the lsb on bit place 2
		c.transferInt |= ((channel<<15) | (0<<14) |  (gain<<13) | (active_mode<<12));   //we manipulate the msb B10100000
		//volatile byte sdsd=channel<<7 | 1<<6 |  gain<<5 | active_mode<<4; 	
		SPI.setClockDivider(SPI_CLOCK_DIV16);
		//SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
		digitalWrite(myDevice,LOW);      // initiate data transfer with 4921
		Serial.println(c.transferInt);
		SPI.transfer(c.aByte[1]);			//send 1st byte
		SPI.transfer(c.aByte[0]);			//send 2nd byte
		digitalWrite(myDevice,HIGH);   // finish data transfer
		return 1;
	}
	else return 0;
}
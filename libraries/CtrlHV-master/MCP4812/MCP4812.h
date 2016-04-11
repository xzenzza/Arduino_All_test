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
#include <Arduino.h>
#include <SPI/SPI.h>
#define READ 0x00

#ifndef MCP4812_H
#define MCP4812_H

class MCP4812
{
	public:
	MCP4812 (uint8_t devicePin, uint8_t myGain);
	
	boolean write(uint8_t channel , uint16_t dacValue);
	private:
	uint8_t myDevice;
	uint8_t gain; 
	uint8_t active_mode;		
	};


#endif
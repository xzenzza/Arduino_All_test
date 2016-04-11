/**********************************************************************************************
 * Arduino CtrlHV - Version 0.02alpha
 * 
 * Copyright (c) 2013 Simon Bauer.  All rights reserved.
 *
 * Short description:
 * The program is designed to control a wood gasifier boiler. The boiler is a suction type. The code may be adapted to pressurized boiler later.
 * The regulating is done by two PIDs from the PID Library from Brett (Thank you!). 
 *
 * Status:
 * Source is debugged and tested with a simulator (on proview.se, Thx for that this piece of epic open source) based on the modbus interface. 
 * So far it is not tested on a real boiler.
 * Hardware layer is missing. Still the one wire is implemented. Use it or throw it.
 * Nearly all comments and variables are German. Sorry for that :(
 * The code is designed for compiling it in AVR Studio. If you want to use the IDE please remove the protos for setup and loop.
 * The code is not yet tested with compiler optimizations.
 * The serialLCD (thx for that) is modified for running on the UART1.
 * There is no user interface. 
 * I'll write a method to write settings received from modubus to eeprom and in case of reset to restore from there.  
 * Not all values are mapped to modbus register. This will be reworked completely later. For now I don't know exactly which value are useful to change. 
 *
 * Changlog 0.02alpha
 * MCP3551 Library is now fully tested and debugged. 
 * 
 *
 * This code is licensed under a GPLv3 License:
 *
 *
 **********************************************************************************************/
#include <CtrlHV.h>


void setup() {
	Serial.begin(9600);
	pinMode(TasteOben, INPUT);
	pinMode(TasteUnten, INPUT);
	pinMode(PumpePin, OUTPUT);
	
	//modbus_configure(9600, 1, 2, TOTAL_REGS_SIZE);
	
	SPI.begin();
	PriLuftRegler.SetSampleTime(priPIDPara.Zeitbasis);
	PriLuftRegler.SetOutputLimits(PriLuftAusgangMin,1023);
	SekLuftRegler.SetSampleTime(priPIDPara.Zeitbasis);
	SekLuftRegler.SetOutputLimits(SekLuftAusgangMin,1023);
	
	
	PriLuftRegler.SetMode(MANUAL);
	SekLuftRegler.SetMode(MANUAL);
}

void loop() {
	unsigned long counter = millis();
	bool rtdReady = getRTD1.getCode();
	if(rtdReady)
	{
		Serial.println(getRTD1.byteCode);
		RTD = calRAdevice1 * (float(getRTD1.byteCode) / ( 2097152.0 - float(getRTD1.byteCode)));
		RTD = (RTD / Re0) - 1;
		Kesseltemperatur = (RTD * (255.8723 + RTD * (9.6 + RTD * 0.878)));
		Serial.print("Temperature: "); Serial.println(Kesseltemperatur);
	}
	if (digitalRead(TasteOben))
	{
		uint8_t n=0;
		float temp=0;
		do 
		{
			if (getRTD1.getCode());
			{
				temp+= float(getRTD1.byteCode);
				n++;	
			}
			
		} while (n<10);
	temp /= 10;	
	calRAdevice1 = Re0;
	calRAdevice1 /= ( temp /( 2097152.0 - temp));
	delay(2000);
	Serial.print("Ra: "); Serial.println(calRAdevice1);
	}
	
	//myLoop();


} //-----------------------END OF LOOP

void myLoop() 
{
	//local variables in loop
	boolean SekRegAktiv =0 , PriRegAktiv=0;
	//-----------------------

	// Place for hardware implementation.
	if (modbusregister[mModbusControl] != 1)
	{
		if (SekRegAktiv) Sauerstoff = 0.0205078125 * analogRead(LambdaSondePin); //We have  21%/1024. This is only for use of a analog lambda device.
		if (PriRegAktiv) Abgastemperatur = random(140,150); //To be changed when hardware is ready.
	}

	// Pumpensteuerung ist unabhängig von der Schrittkette-------------------------------------------------------
	if ( hysteresis(Kesseltemperatur,KTempPumpeAn,2,1) > KTempPumpeAn ) //Die Pumpe ist immer an wenn >
	{
		digitalWrite(PumpePin,statusPumpe = 1);
		statusPumpenWaechter = 1;
	}
	else if (statusPumpenWaechter && Kesseltemperatur < KTempPumpeAn )
	{
		digitalWrite(PumpePin,statusPumpe = 0);
		statusPumpenWaechter = 0;
	}
	if (Schrittkette != 0 && hysteresis(Kesseltemperatur,HeizbetriebPumpeAn,1,1) > HeizbetriebPumpeAn ) digitalWrite(PumpePin, statusPumpe = 1);

	else if (hysteresis(Kesseltemperatur,HeizbetriebPumpeAn - 2,1,0) < HeizbetriebPumpeAn - 1) digitalWrite(PumpePin, statusPumpe = 0);

	// Pumpensteuerung Ende--------------------------------------------------------------------------------------

	// Übergeordneter Temperaturwächter------------------------------------------------------------------------------
	// Ein typgeprüft STB für das Gebläse ist weiterhin erforderlich!!!
	// Dieser ist nur aktiv wenn das Saugzuggebläse angesteuert wird. Ansonsten übernimmt diese Aufgabe die Kesselsteuerung des Herstellers.
	unsigned long recentMillis = millis();
	if ( Kesseltemperatur >= KesseltemperaturMax && (recentMillis - lastWaechterTime) > 15000)
	//das heißt nach 20 x 15 = 5 Minuten ist das Gebläse bei Null
	{
		lastWaechterTime = millis();
		dac2.write(SaugZug, SaugZugAusgang -= fuenfProzent);
		statusTempWaechter = 1;
	}
	else if (statusTempWaechter && hysteresis(Kesseltemperatur,KesseltemeraturMaxMax,2,1) > KesseltemeraturMaxMax) dac2.write(SaugZug,SaugZugAusgang = SaugZugAusgangMin); //harte Rücknahme auf minimal Gebläse
	else if (statusTempWaechter && (Kesseltemperatur < KesseltemperaturMax))
	{
		dac2.write(SaugZug,SaugZugAusgang += fuenfProzent);
		if (SaugZugAusgang == hundertProzent ) statusTempWaechter = 0;	
	}

	//Schrittkette der Vergaserregelung--------------------------------------------------------------------------------------------------
	/*Nur für Saugzug. Die Stellglieder sind die Luftklappen des Vergasers. Die Herstellersteuerung bleibt weiterhin aktiv und wird zur Rücknahme des Gebläses benutzt.
	Alternativ kann auch das Gebläse über den Pin SaugZugPin stetig angesteuert werden (Schritte mit "informativ" gekennzeichnet.
	*/
	//Anzünden muss sein ;)
	if (Schrittkette == 0)
	{
		if (btnStartVergaser && !statusAnheizklappe) dac2.write(SaugZug,SaugZugAusgang = SaugZugStart); // Diesen Befehl drin lassen oder durch einen Merker ersetzen
	
		if (statusPriTuer && SaugZugAusgang > 0)
		{
			dac1.write( PriLuft, PriLuftAusgang = startWertPriLuft); //Für das Intervall StartTimer wird die Primärluft gedrosselt.
			if (delaySeconds(1,startZeitPriluft)) //Generelle Erklärung: Wenn die erste Abfrage wahr wird dann erst wird der Timer gestartet.
			{
				Schrittkette = 1;
				SchrittkettenZeit = millis();
			}
		}		
	}

	// Anheizklappenüberwachung
	/*recentMillis = millis();
	if (!statusAnheizklappe && Schrittkette == 1 && (recentMillis - SchrittkettenZeit) > maxZeitAnheizklappeOffen) ; //Serial.println("Alarm Schrittkette 1"); //Alarm setzen wenn nach x Sekunden die Anheizklappe nicht geschloßen wurde.
	else if (Schrittkette == 1 && statusAnheizklappe)  ;//Serial.println("."); //kein Alarm
	*/


	/*Anheizen
	* Um einen möglichst schnelles aufheizen zu ermöglichen, wird die PriKlappe maximal geöffnet und gewartet bis sich eine Temperatur nahe dem Sollwert AGT einstell.
	* Um die Verbrennung auch während des Anheizens optimal zu unterhalten ist die Lambdaregelung aktiv. 
	* Es kann evtl. notwendig sein beim Übergang zum Heizbetrieb die PriKlapp um einen Teil zu schließen um ein Überschwingen der AGT zu vermeiden. 
	*/
	if (Schrittkette == 1)
	{	
		SekLuftRegler.SetMode(AUTOMATIC);
		SekRegAktiv = SekLuftRegler.Compute();  //Starte Lambdaregelung
	
		if (SekRegAktiv) //Die Überwachungskette wird immer nur bei einem wahren compute durchgeführt.
		{
			dac1.write(SekLuft,SekLuftAusgang);
			dac2.write(SaugZug,SaugZugAusgang = hundertProzent); //Gebläse auf max. derzeit nur Informativ. 	
	
			if ( Sauerstoff >= SauerstoffMinMin && !statusRuecknahme ) dac1.write(PriLuft, PriLuftAusgang = hundertProzent);
			else if ( SekLuftAusgang > 1020 ) // wir warten erst bis die Klappe wirklich voll offen steht. 
			{
				if (delaySeconds(1,1)) //Hier wird der Gradient zeitmäßig auf 1 Sekunde pro Einheit begrenzt. Je nach Zustand des Reglers kann dies aber auch bis zu 1 Sekunde + Zeitbasis Regler dauern.
				{
				if (PriLuftAusgang > PriLuftAusgangMin) dac1.write(PriLuft, PriLuftAusgang -= maxPriLuftGradient); //Rücknahme wenn Sauerstoff zu niedrig ist bei voll offener Sekundärluft. 
				statusRuecknahme = 1;
				}			
			}	
			else if (statusRuecknahme == 1 && Sauerstoff >= SauerstoffMin) dac1.write(PriLuft, PriLuftAusgang); //auf letzten gültigen Wert verharren bis Schrittende
		  
			if (Abgastemperatur >= AbgastemperaturSollwert - 5) //Der Übergang in den Heizbetrieb erfolgt unterhalb des Abgassollwerts um dem Regler genügend Raum zu geben, denn Priluft ist immer noch auf max.
			{
				if (delaySeconds(2,5)) //nach 5 Sekunden den Heizbetrieb erkennen.
				{	
					// analogWrite(PriLuftPin, ); // Hier evtl. PriKlappe zurücknehmen.
					Schrittkette = 2;
					statusRuecknahme = 0;
				}			
			}
		}
	}	

	//Heizbetrieb
	if (Schrittkette == 2)
	{
		//Primärluftregelung
		PriLuftRegler.SetMode(AUTOMATIC);
		PriRegAktiv =PriLuftRegler.Compute();
		if (PriRegAktiv) 
			{
			dac1.write(PriLuft, PriLuftAusgang);
			if (Sauerstoff <= SauerstoffMin && SekLuftAusgang >= achtundneunzigProzent)  //Rücknahme der Primärluft zu gunsten Sauerstoffrest
			{
				if (!statusRuecknahme) oldAbgasTemperaturSollwert = AbgastemperaturSollwert; //alten Wert speichern
				if (AbgastemperaturSollwert > AbgastemperaturAusschaltWert) AbgastemperaturSollwert -= AbgastemperaturRuecknahme;  //Rücknahme
				statusRuecknahme = 1;
			}
			else if (Sauerstoff > SauerstoffMin && AbgastemperaturSollwert < oldAbgasTemperaturSollwert) //Sollwert wieder Schrittweise anheben auf alten Wert
			{
				AbgastemperaturSollwert += AbgastemperaturRuecknahme;
			}
			else statusRuecknahme = 0;
		
			// Führungsgröße erkennen ansonsten Fehler werfen
		}	
	
		//Sekundärluftregelung
		boolean SekRegAktiv = SekLuftRegler.Compute();
		if (SekRegAktiv)
		{
			dac1.write(SekLuft, SekLuftAusgang);
			if (Abgastemperatur < AbgastemperaturAusschaltWert && (PriLuftAusgang > achtundneunzigProzent || SekLuftAusgang > achtundneunzigProzent))
			{
			 if (delaySeconds(2,10)) Schrittkette =3; //Ausbrand nach 10 Sekunden erkennen
			}
		// Führungsgröße erkennen ansonsten Fehler werfen	
		}
	}
	//Ausbrand
	if (Schrittkette == 3)
	{	
		PriLuftRegler.SetMode(MANUAL);
		SekLuftRegler.SetMode(MANUAL);
		dac1.write(PriLuft, PriLuftAusgang = OFF);
		dac1.write(SekLuft, SekLuftAusgang= OFF);
		dac2.write(SaugZug, SaugZugAusgang = OFF); //nur informativ
		if (Kesseltemperatur < AusbrandPumpeAus)
		{
			Schrittkette = 0;
			digitalWrite(PumpePin, statusPumpe = 0);
		}	
	}

	// Betriebsstörung
	if (Schrittkette == 99)
	{ //do something important
	}

	 //check for new data on modbus
	int actualModbusControll = updateModbusRegs();
	if (actualModbusControll != 0) //When ever it is not 0 we have to do some further steps:
	{
	computeModbusData(); //we compute it
	}
	else if (actualModbusControll == 4)
	{
		// save settings to eeprom
	}
	// Sometime here may be the place for the GUI :)
}




int main(void)
{
	init();

	#if defined(USBCON)
	USBDevice.attach();
	#endif
	
	setup();
	
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
	
	return 0;
}

int floatToModbusInt(const float &Float)
//int = int (float * 100)
{
	return int((Float * 100));
}

int percentToByte( int percent )
{
	if (percent == 100) return 1023;
	else return 10.24 * percent;
}

int byteToPercent( int mybyte )
{
	if(mybyte > 1020) return 100;
	else return mybyte / 10.24;
}
int updateModbusRegs()
{
	/* modbusregister[mModbusControl]
		where
		0 = Read process values only.
		1 = Simulation of process values i.e. write Values.
		2 = Write settings.
		3 = Read settings.
		4 = Write settings. To eeprom.
		*/

	int myStatus = 0b000000000000000;
	int n = 0;
	//Update all the registers
	//Process values	
	if (modbusregister[mModbusControl] != 1) //Will be only updated when simulation is not active (bidirectional values)
	{
	//Mask statusbooleans to integer
	if(statusRuecknahme) bitSet(myStatus,n);
	n++;
	if(statusPumpe) bitSet(myStatus,n);
	n++;
	if(statusPumpenWaechter) bitSet(myStatus,n);
	n++;
	if(statusTempWaechter) bitSet(myStatus,n);
	n++;
	if(statusPriTuer) bitSet(myStatus,n);
	n++;
	if(statusAnheizklappe) bitSet(myStatus,n);
	n++;
	if(btnStartVergaser) bitSet(myStatus,n);
	n = 0;		
	modbusregister[mstatusBits]			 = myStatus;	
	modbusregister[mAbgastemperatur]	 = floatToModbusInt(Abgastemperatur);
	modbusregister[mRuecklauftemperatur] = floatToModbusInt(Ruecklauftemperatur);
	modbusregister[mKesseltemperatur]	 = floatToModbusInt(Kesseltemperatur);
	modbusregister[mSauerstoff]			 = floatToModbusInt(Sauerstoff);
	}	
	modbusregister[mPriLuftAusgang]		 = byteToPercent(PriLuftAusgang);
	modbusregister[mSekLuftAusgang]		 = byteToPercent(SekLuftAusgang);
	modbusregister[mSaugzugAusgang]		 = byteToPercent(SaugZugAusgang);
	modbusregister[mSchrittkette]		 = Schrittkette;
	//Settings
	if(modbusregister[mModbusControl] == 3 ) //to save some time this will only executed when the operator needs the data. Write 3 to ModbusControl and read the data in the next circle
	{
	modbusregister[mAbgastemperaturSollwert]		= AbgastemperaturSollwert;
	modbusregister[mSauerstoffSollwert]				= SauerstoffSollwert;
	modbusregister[mAbgastemperaturRuecknahme]		= AbgastemperaturRuecknahme;
	modbusregister[mAbgastemperaturMax]				= AbgastemperaturMax;
	modbusregister[mAbgastemperaturAusschaltWert]	= AbgastemperaturAusschaltWert;
	modbusregister[mSauerstoffMin]					= SauerstoffMin;
	modbusregister[mSauerstoffMinMin]				= SauerstoffMinMin;
	modbusregister[mKesseltemperaturEinWert]		= KesseltemperaturEinWert;
	modbusregister[mKesseltemperaturMax]			= KesseltemperaturMax;
	modbusregister[mKesseltemeraturMaxMax]			= KesseltemeraturMaxMax;
	modbusregister[mKTempPumpeAn]					= KTempPumpeAn;
	modbusregister[mHeizbetriebPumpeAn]				= HeizbetriebPumpeAn;
	modbusregister[mAusbrandPumpeAus]				= AusbrandPumpeAus;
	modbusregister[mstartWertPriLuft]				= startWertPriLuft;
	modbusregister[mSaugZugStart]					= byteToPercent(SaugZugStart);
	modbusregister[mPriLuftAusgangMin]				= byteToPercent(PriLuftAusgangMin);
	modbusregister[mmaxPriLuftGradient]				= byteToPercent(maxPriLuftGradient);
	modbusregister[mSekLuftAusgangMin]				= byteToPercent(SekLuftAusgangMin);
	modbusregister[mstartZeitPriluft]				= startZeitPriluft;
	modbusregister[mmaxZeitAnheizklappeOffen]		= maxZeitAnheizklappeOffen;
	modbusregister[mPriRegP]						= priPIDPara.PAnteil; //31
	modbusregister[mPriRegI]						= priPIDPara.IAnteil;//32
	modbusregister[mPriRegD]						= priPIDPara.DAnteil;//33
	modbusregister[mPriRegTim]						= priPIDPara.Zeitbasis; //34
	modbusregister[mSekRegP]						= sekPIDPara.PAnteil;//35
	modbusregister[mSekRegI]						= sekPIDPara.IAnteil;//36
	modbusregister[mSekRegD]						= sekPIDPara.DAnteil;//37
	modbusregister[mSekRegTim]						= sekPIDPara.Zeitbasis;//38
	}	
// Compute all Modbusdata
modbus_update(modbusregister);

//when ever we have a >0 there must be some addional steps
return modbusregister[mModbusControl];
}

int computeModbusData()
{	//compute new data
	//returns 1 when new settings are written	
		boolean stbool;
		int buffReg;
		int myStatus,n;
		if (modbusregister[mModbusControl] == 1) //Simulation
		{
			
			myStatus = modbusregister[mstatusBits];
			n = 0;
			if ((stbool = bitRead(myStatus,n))) btnStartVergaser = stbool;
			else btnStartVergaser = 0;
			n++;
			if ((stbool = bitRead(myStatus,n))) statusPriTuer = stbool;
			else statusPriTuer = 0;
			n++;
			if ((stbool = bitRead(myStatus,n))) statusAnheizklappe = stbool;
			else statusAnheizklappe = 0;
			
			Abgastemperatur = float(modbusregister[mAbgastemperatur]);
			Ruecklauftemperatur = float(modbusregister[mRuecklauftemperatur]);
			Kesseltemperatur = float(modbusregister[mKesseltemperatur]);
			Sauerstoff = float(modbusregister[mSauerstoff]);
		}
				
		if (modbusregister[mModbusControl] == 2) //Operator wants to write values. All values should be !=0!
		{
			if ( (buffReg = modbusregister[mAbgastemperaturSollwert]) > 0 )		AbgastemperaturSollwert = buffReg;
			if ( (buffReg = modbusregister[mSauerstoffSollwert]) > 0 )			SauerstoffSollwert = buffReg;
			if ( (buffReg = modbusregister[mAbgastemperaturRuecknahme]) > 0 )	AbgastemperaturRuecknahme = buffReg;
			if ( (buffReg = modbusregister[mAbgastemperaturMax]) > 0)			AbgastemperaturMax = buffReg;
			if ( (buffReg = modbusregister[mAbgastemperaturAusschaltWert]) > 0) AbgastemperaturAusschaltWert = buffReg;
			if ( (buffReg = modbusregister[mSauerstoffMin]) > 0)				SauerstoffMin = buffReg;
			if ( (buffReg = modbusregister[mSauerstoffMinMin]) > 0)				SauerstoffMinMin = buffReg;
			if ( (buffReg = modbusregister[mKesseltemperaturEinWert]) > 0)		KesseltemperaturEinWert = buffReg;
			if ( (buffReg = modbusregister[mKesseltemperaturMax]) > 0)			KesseltemperaturMax = buffReg;
			// const! if ( buffReg = modbusregister[mKesseltemeraturMaxMax] > 0) KesseltemperaturMaxMax
			if ( (buffReg = modbusregister[mKTempPumpeAn]) > 0)					KTempPumpeAn = buffReg;
			if ( (buffReg = modbusregister[mHeizbetriebPumpeAn]) > 0)			HeizbetriebPumpeAn = buffReg;
			if ( (buffReg = modbusregister[mAusbrandPumpeAus]) > 0)				AusbrandPumpeAus = buffReg;
			if ( (buffReg = modbusregister[mstartWertPriLuft]) > 0)				startWertPriLuft = buffReg;
			if ( (buffReg = modbusregister[mSaugZugStart]) > 0)					SaugZugStart = buffReg;
			if ( (buffReg = modbusregister[mPriLuftAusgangMin]) > 0)			PriLuftAusgangMin = percentToByte(buffReg);
			if ( (buffReg = modbusregister[mSekLuftAusgangMin]) > 0)			SekLuftAusgangMin = percentToByte(buffReg);
			if ( (buffReg = modbusregister[mstartZeitPriluft]) > 0)				startZeitPriluft = buffReg;
			if ( (buffReg = modbusregister[mmaxZeitAnheizklappeOffen]) > 0)		maxZeitAnheizklappeOffen = buffReg;
			if ( (buffReg = modbusregister[mPriRegP]) > 0)						priPIDPara.PAnteil = buffReg; //31
			if ( (buffReg = modbusregister[mPriRegI]) > 0)						priPIDPara.IAnteil = buffReg;//32
			if ( (buffReg = modbusregister[mPriRegD]) > 0)						priPIDPara.DAnteil = buffReg;//33
			if ( (buffReg = modbusregister[mPriRegTim]) > 0)					priPIDPara.Zeitbasis = buffReg; //34
			if ( (buffReg = modbusregister[mSekRegP]) > 0)						sekPIDPara.PAnteil = buffReg;//35
			if ( (buffReg = modbusregister[mSekRegI]) > 0)						sekPIDPara.IAnteil = buffReg;//36
			if ( (buffReg = modbusregister[mSekRegD]) > 0)						sekPIDPara.DAnteil = buffReg;//37
			if ( (buffReg = modbusregister[mSekRegTim]) > 0)					sekPIDPara.Zeitbasis = buffReg;//38
			
			//Now we may initialsize the new values
			
			
			//you must set Sample time prior to tunings. Otherwise the tunings are calculated wrong!
			PriLuftRegler.SetSampleTime(priPIDPara.Zeitbasis);
			PriLuftRegler.SetTunings(priPIDPara.PAnteil,priPIDPara.IAnteil,priPIDPara.DAnteil);
			SekLuftRegler.SetSampleTime(sekPIDPara.Zeitbasis);
			SekLuftRegler.SetTunings(sekPIDPara.PAnteil,sekPIDPara.IAnteil,sekPIDPara.DAnteil);
			return 1;
		}		

return 0;
}

int hysteresis(int actualValue, int setpoint, int hystersis, int direction)
{
	if (actualValue <= setpoint + hystersis && actualValue >= setpoint - hystersis)
	{
		if (direction==0) return setpoint-hystersis;
		else if (direction==1) return setpoint+hystersis;
		else return actualValue;
	}
	else
	 {
		 return actualValue;
	 }
}

int delayMilliSeconds(int timerNumber,unsigned long delaytime){
	unsigned long timeTaken;
	if (timerState[timerNumber]==0){    //If the timer has been reset (which means timer (state ==0) then save millis() to the same number timer,
	timer[timerNumber]=millis();
	timerState[timerNumber]=1;      //now we want mark this timer "not reset" so that next time through it doesn't get changed.
}
if (millis()> timer[timerNumber]){
	timeTaken=millis()+1-timer[timerNumber];    //here we see how much time has passed
}
else{
	timeTaken=millis()+2+(4294967295-timer[timerNumber]);    //if the timer rolled over (more than 48 days passed)then this line accounts for that
}
if (timeTaken>=delaytime) {          //here we make it easy to wrap the code we want to time in an "IF" statement, if not then it isn't and so doesn't get run.
timerState[timerNumber]=0;  //once enough time has passed the timer is marked reset.
return 1;                          //if enough time has passed the "IF" statement is true
  }
  else {                               //if enough time has not passed then the "if" statement will not be true.
    return 0;
  }
}
int delayHours(byte timerNumber,unsigned long delaytimeH){    //Here we make it easy to set a delay in Hours
return delayMilliSeconds(timerNumber,delaytimeH*1000*60*60);
}
int delayMinutes(byte timerNumber,unsigned long delaytimeM){    //Here we make it easy to set a delay in Minutes
return delayMilliSeconds(timerNumber,delaytimeM*1000*60);
}
int delaySeconds(byte timerNumber,unsigned long delaytimeS){    //Here we make it easy to set a delay in Seconds
return delayMilliSeconds(timerNumber,delaytimeS*1000);
}
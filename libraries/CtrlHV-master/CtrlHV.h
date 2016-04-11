/*-----------------------------
* See header in CtrlHV.cpp
*
*
*/

#include <Arduino.h>
#include <safeTemperature/safeTemperature.h>
#include <stdint.h>
#include <serLCD/serLCD.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <SimpleModbusSlave.h>
#include <Bounce/Bounce.h>
#include <PID_v1/PID_v1.h>
#include <EEPROM/EEPROM.h>
#include <EEPROMEx/EEPROMEx.h>
#include <MCP3551/MCP3551.h>
#include <MCP4812/MCP4812.h>


//Definitionen
// Ausgabepins----------------
// Analog IN
#define				LambdaSondePin 0
#define				AbgastemperaturPin 1
#define				KesseltemperaturPin 2
#define				RuecklauftemperaturPin 3  //may be with one wire
#define				BrennraumtemperaturPin 4

// Analog OUT
#define				PriLuft 0
#define				SekLuft 1
#define				SaugZug 0


// Digital OUT
#define				PumpePin 23

//SPI SLAVE SELECT PINS
#define				MCP3551Device1 30
//#define				MCP3551Ch2 31
//#define				MCP3551Ch3 32
#define				DAC1Pin 46
#define				DAC2Pin 53

// Digital IN
#define				TasteOben 40
#define				TasteUnten 41


// Sonstiges----
#define				OFF 0
#define				Re0	100

// Sollwerte----------------

// Zahlen
#define				hundertProzent	1023
#define				achtundneunzigProzent 1003
#define				fuenfProzent	51
//-------------------------
//Globale Variablen
boolean			statusRuecknahme;					// 1 = Aktiv
boolean			statusPumpenWaechter;				// 1 = Aktiv t > 
boolean			statusTempWaechter;

int				oldAbgasTemperaturSollwert;			//undefiniert. Wird benutzt um den aktuellen Sollwert zu sichern.
uint8_t			Schrittkette = 0;					// Schrittkette 0...3
int				newModbusSettings;

unsigned long	SchrittkettenZeit;					//undefiniert
unsigned long	lastWaechterTime = 0;				//undefiniert

float			RTD=0;								//temp
float		calRAdevice1 = 13607;				//calibrated value for RTD calculation 13607 pt100
// Sollwerte:
double			SauerstoffSollwert = 6;				//Sollwert für Sauerstoff während dem Heizbetrieb
double			AbgastemperaturSollwert = 170;		//Sollwert Abgastemperatur

struct PIDPara {
	double PAnteil;
	double IAnteil; //ki = 
	double DAnteil;
	double Zeitbasis;
}	sekPIDPara = {
	10,
	1,
	1,
	1*1000}, priPIDPara = {
	10,
	1,
	1,
	1*1000};

//Limits 
double			AbgastemperaturRuecknahme = 1;		//um x Grad wird die Temperatur zurückgenommen wenn Sauerstoff zu wenig. Das Äquivalent zur reinen Rücknahme des Ausgangswert. Hier nur kleine Werte sinnvoll da ansonsten der Regler einen großen Sprung sieht.
int				maxPriLuftGradient = 1 * (1024 / 100); // Dieser Gradient nimmt die Primäluft zurück in %/Sekunde. ein (1) Prozent entspricht dem hundertsten Teil von 256.

int				AbgastemperaturMax;
int				AbgastemperaturAusschaltWert = 120; //Abschaltwert bei Ausbrand

int				SauerstoffMin = 2;					//aktiviert Rücknahme beim Heizen
int				SauerstoffMinMin = 1;				//aktiviert Rücknahme beim Anheizen

int				KesseltemperaturEinWert = 65;		// Schaltet die Umwälzpumpe ein
int				KesseltemperaturMax = 87;			// Nimmt das Gebläse hart zurück
int	const		KesseltemeraturMaxMax = 90;			// Erstickt den Kessel
int				KTempPumpeAn = 85;					// Ab dieser Temperatur ist die Umwälzpumpe immer an.
int				HeizbetriebPumpeAn = 68;			// Ab dieser Temperatur wir die Umwälzpumpe eingeschaltet wenn im Heizbetrieb.
int				AusbrandPumpeAus = 82;				// Ab dieser Temperatur wird die Umwälzpumpe ausgeschaltet wenn im Ausbrand.

uint16_t		startWertPriLuft = 70 * (1024/100);	//integer Pri Luft bei Start.
uint16_t		SaugZugStart = 30 * (1024/100);		//integer 

uint16_t		PriLuftAusgangMin = 20 * (1024/100);	//integer Im Regelbetrieb wird dieser Wert nie unterschritten
uint16_t		SekLuftAusgangMin = 20 * (1024/100);	//integer  
uint16_t		SaugZugAusgangMin = 20 * (1024/100);	//integer value! 

int				startZeitPriluft = 5;				//  Sekunden Anzündphase nach Tür geschloßen
unsigned long	maxZeitAnheizklappeOffen = 300000;	// Wie lange darf die Tür offen stehen

//I/O:
uint16_t		SaugZugAusgang = 0;					//Anloger Ausgang des Saugzuggeblässes 0...1023

boolean			btnStartVergaser;					//Startbutton
boolean			statusAnheizklappe;					// 1 wenn geschloßen
boolean			statusPriTuer;						// 1 wenn geschloßen

boolean			statusPumpe =0;						//Pumpenausgang

double			Sauerstoff;							//Aktueller Sauerstoff-Rest
double			PriLuftAusgang;						//Analoger Ausgang der Primärluftklappe 0...1023
double			Abgastemperatur;					//Aktuelle Abgastemperatur
double			SekLuftAusgang;						//Analoger Ausgagng der Sekundärluftklappe 0...1023

float			Kesseltemperatur;					//Aktuelle Kesseltemperatur
float			Ruecklauftemperatur;				//Akutelle Rücklauftemperatur
float			Brennraumtemperatur;				//Aktuelle Brennraumtemperatur

//Sonstiges
char			buffChar[4];						//Char Buffer für Display

//Displaytexte
char const		Zeile1[] = {"O2     % AGT    °"};
char const		Zeile2[] = {"O2s    % AGs    °"};
char const		Zeile3[] = {"KT     ° RL     °"};
char const		Zeile4[] = {"Sch    ° Feh    °"};



//----------------------------


//Objekte-----------------------------------------------------------------------------------
unsigned long timer[4];
byte timerState[4];


serLCD				myLCD; //serielles Display am UART1, die Libary wurde angepaßt damit man mit dem Hardware UART arbeiten kann.

Bounce				TasterOben(TasteOben, 10);
Bounce				TasterUnten(TasteUnten, 10);

PID					PriLuftRegler(&Abgastemperatur, &PriLuftAusgang,	&AbgastemperaturSollwert ,	priPIDPara.PAnteil, priPIDPara.IAnteil, priPIDPara.DAnteil,DIRECT);
PID					SekLuftRegler(&Sauerstoff,		&SekLuftAusgang,	&SauerstoffSollwert,		sekPIDPara.PAnteil,	sekPIDPara.IAnteil, sekPIDPara.DAnteil,DIRECT);

MCP3551				getRTD1(MCP3551Device1);
MCP3551				getRTD2(31);
//DACs
MCP4812				dac1(DAC1Pin,1);
MCP4812				dac2(DAC2Pin,1);
//-----------------------------------------------------------------------------------------------

//Modbusregister--------------------------------------------
enum
{
	mModbusControl, //Adr: 0
	mstatusBits,			//1	
	mAbgastemperatur,//2
	mRuecklauftemperatur,//3
	mKesseltemperatur,//4
	mSauerstoff,//5
	mPriLuftAusgang,//6
	mSekLuftAusgang,//7
	mSaugzugAusgang,//8
	mSchrittkette,//9 
	
	mNOTUSED,				//10		
	mAbgastemperaturSollwert,//11
	mSauerstoffSollwert,//12
	mAbgastemperaturRuecknahme,//13
	mAbgastemperaturMax,//14
	mAbgastemperaturAusschaltWert,//15
	mSauerstoffMin,//16
	mSauerstoffMinMin,//17
	mKesseltemperaturEinWert,//18
	mKesseltemperaturMax,//19
	mKesseltemeraturMaxMax,//20
	mKTempPumpeAn,//21
	mHeizbetriebPumpeAn,//22
	mAusbrandPumpeAus,//23
	mstartWertPriLuft,//24
	mSaugZugStart,//25
	mPriLuftAusgangMin,//26
	mmaxPriLuftGradient,//27
	mSekLuftAusgangMin,//28
	mstartZeitPriluft,//29
	mmaxZeitAnheizklappeOffen,//30
	mPriRegP,//31
	mPriRegI,//32
	mPriRegD,//33
	mPriRegTim,//34
	mSekRegP,//35
	mSekRegI,//36
	mSekRegD,//37
	mSekRegTim,//38
	// leave this one
	TOTAL_REGS_SIZE
	// total number of registers for function 3 and 16 share the same register array
};
int modbusregister[TOTAL_REGS_SIZE];

//Prototypes------------------------------
int main(void);
void setup();
void loop();
void myLoop();
int floatToModbusInt(const float &Float);
int percentToByte( int percent );
int byteToPercent( int mybyte );
int updateModbusRegs();
int computeModbusData();
int hysteresis(int actualValue, int setpoint, int hystersis, int direction);
int delayMilliSeconds(int timerNumber,unsigned long delaytime);
int delayHours(byte timerNumber,unsigned long delaytimeH);
int delayMinutes(byte timerNumber,unsigned long delaytimeM);
int delaySeconds(byte timerNumber,unsigned long delaytimeS);

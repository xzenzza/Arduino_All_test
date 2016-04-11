// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
#define DHTPIN 3
#define DHTTYPE DHT22

unsigned int index;



OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer, outsideThermometer,uppersideThermometer,lowwersideThermometer;
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(void)
{

  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  sensors.begin();
  dht.begin();
  lcd.begin();
  lcd.backlight();

  lcd.print("   Pls Wait"); 
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode())
  Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1"); 
  if (!sensors.getAddress(uppersideThermometer, 2)) Serial.println("Unable to find address for Device 3"); 
  if (!sensors.getAddress(lowwersideThermometer, 3)) Serial.println("Unable to find address for Device 4"); 

  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(outsideThermometer);
  Serial.println();

    Serial.print("Device 3 Address: ");
  printAddress(uppersideThermometer);
  Serial.println();

  Serial.print("Device 4 Address: ");
  printAddress(lowwersideThermometer);
  Serial.println();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(uppersideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(lowwersideThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC); 
  Serial.println();

  Serial.print("Device 3 Resolution: ");
  Serial.print(sensors.getResolution(uppersideThermometer), DEC); 
  Serial.println();

  Serial.print("Device 4 Resolution: ");
  Serial.print(sensors.getResolution(lowwersideThermometer), DEC); 
  Serial.println();
  delay(1000);
  lcd.clear(); 
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
  
  

  
  //lcd.clear();
  //lcd.print("Station : ");
  //lcd.print(index);
  lcd.setCursor(0,1);
  lcd.print("Temp : ");
  lcd.print(tempC);
  lcd.print(" *C");
  //lcd.setCursor(0,1);
  //lcd.print(" Temp F: ");
 // lcd.print(DallasTemperature::toFahrenheit(tempC));
  delay(1000);
  lcd.clear(); 
    

  
  
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

/*
 * Main function, calls the temperatures in a loop.
 */
void loop(void)
{ 
  lcd.print("   Reading Temp");
delay(1000);
  lcd.clear();
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  lcd.print("Station : 1 ");
  printData(insideThermometer);
  lcd.print("Station : 2 ");
  printData(outsideThermometer);
  lcd.print("Station : 3 "); 
  printData(uppersideThermometer);
  lcd.print("Station : 4 "); 
  printData(lowwersideThermometer);
  //lcd.print(h);
  //delay(1000);
  lcd.clear(); 


  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  //-
  lcd.print("Station : 5 ");
  lcd.setCursor(0,1);
  lcd.print("Read Hum & Temp");
  delay(500);
  lcd.clear();
  lcd.print("Humid: ");
  lcd.print(h);
  lcd.print(" %\t");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" *C ");
  //--
 
  //lcd.print(h);
  delay(1000); 
   lcd.clear();
}

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include "DHT.h"
#include "EmonLib.h"                   // Include Emon Library

//----------------------Pin_Read_DHT22-----------//
#define DHT1PIN D2
#define DHT2PIN D1
#define DHT1TYPE DHT22
#define DHT2TYPE DHT22

//----------------------Pin_contron--------------//
#define Relay_1 D4
#define Relay_2 D5
#define Relay_3 D6

//--------------------Seve_value_Analog---------------//
float Analog_V_Fan,GetAnalog_Voltage_Fan;
float Analog_I_Fan,GetAnalog_Current_Fan;
float Analog_I_Com;
unsigned int Noise_LV;
float Analog_Noise;
double AmpsRMS = 0;


//--------------------Seve_value_Temp_Humidity_for_DHT22---------------//
float h1,t1,h2,t2;


//--------------------Set_value_connet_Router__and__upload_Thingspeak------//

//const char* ssid     = "C&amp;amp;EST_RMUTI";
//const char* ssid     = "Internet_of_Farm";
//const char* password = "123467890";

const char* ssid     = "Smart-Soul";
//const char* ssid     = "Internet_of_Farm";
const char* password = "86Irit86h,8iv'";
//******************WiFi***************************//
//const char* ssid     = "Smart-Soul";
//const char* password = "86Irit86h,8iv'";

const char* host = "api.thingspeak.com";

//-----------------Function_Read_DHT22-----------//

DHT dht1(DHT1PIN, DHT1TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);

//--------USE EmonLib For caculate current------//

EnergyMonitor emon1;                   // Create an instance
double Irms;

//-----------------Function_Set_for_Arduino-------//

void setup() {

  //-------------------------Using_Pin_Contron_and_LED_Status---------------//
  pinMode(Relay_1, OUTPUT);
  pinMode(Relay_2, OUTPUT);
  pinMode(Relay_3, OUTPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  //------------------------------DHT22--------------------------//
  dht1.begin();
  dht2.begin();
emon1.current(A0,15.151515);             // Current: input pin, calibration. 31.0303
 for(int Calibrate = 0 ; Calibrate<10; Calibrate++ )
 {
   Irms = emon1.calcIrms(1480);  // Calculate Irms only
  Serial.println(Irms);          // Irms
  delay(50);
 }

  //--------------------Tell_Mornitor_--------------//
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  //---------------------Connet_Router--------------------//
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  //-----------------------------------------------------//
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  //--------------------------Conneting----------------------//
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//--------------------------------------------------------------------//

//----------------------------LED Status Working-------------------------------------------//
void Tell_Status_Work(){

  digitalWrite(BUILTIN_LED,LOW); delay(100);
  digitalWrite(BUILTIN_LED,HIGH); delay(100);
}


float getVPP()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here

   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(A0);
       // see if you have a new maxValue
       if (readValue > maxValue)
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue)
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }

   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;

   return result;
 }
void get_CurrentRMSR()
{
double Voltage = 0;
double VRMS = 0;

   Voltage = getVPP();
   VRMS = (Voltage/2.0) *0.707;
   AmpsRMS = (VRMS * 1000)/100;
   if(AmpsRMS<=0.15)AmpsRMS = 0.00;
   Serial.print(AmpsRMS);
   Serial.println(" Amps RMS");

}

void Function_Work(){
//---------------------------------------------------------------------//
//***************************************************////----------------------- Function Number 1 ---------------------------

//**READ CT Current Sensor***//
  digitalWrite(Relay_1, LOW);
  digitalWrite(Relay_2, LOW);
  digitalWrite(Relay_3, HIGH);
  delay(100);
   // emon1.current(A0,30.0303);             // Current: input pin, calibration.
 for(int Calibrate = 0 ; Calibrate<5; Calibrate++ )
 {
   Irms = emon1.calcIrms(1480);  // Calculate Irms only
  Serial.println(Irms);          // Irms
  delay(10);
 }
 Serial.print(" CT Read Value : ");
  Serial.print(analogRead(A0));
  Irms = emon1.calcIrms(1480);  // Calculate Irms only
  Serial.print("\t");
  Serial.print(" A = ");
  Serial.println(Irms);          // Irms
  delay(1000);

  Tell_Status_Work();

//-----------------------------------------------------//
//***READ Voltage Fan***-//****************************//////--------------------- Function Number 2 ---------------------------

  digitalWrite(Relay_1, LOW);
  digitalWrite(Relay_2, HIGH);
  digitalWrite(Relay_3, HIGH);
  delay(100);
  Serial.print(" Voltage Value : ");


  Serial.print(analogRead(A0));
  Serial.print("\t");
  GetAnalog_Voltage_Fan = analogRead(A0);
  if(GetAnalog_Voltage_Fan <= 50) GetAnalog_Voltage_Fan = 0;
  Serial.print(" Voltage :  ");
  Serial.print(GetAnalog_Voltage_Fan/4.36363636);
  Serial.println(" Volt ");
  Analog_V_Fan = int(GetAnalog_Voltage_Fan/4.36363636);
  delay(2000);
  Tell_Status_Work();

//-----------------------------------------------------//
//*****READ ACS712 30A Current Sensor**///*******************///---------------------- Function Number 3 ---------------------------

  digitalWrite(Relay_1, LOW);
  digitalWrite(Relay_2, LOW);
  digitalWrite(Relay_3, LOW);
  delay(100);
 /* Serial.print(" Current Value : ");
 float Get_amp = analogRead(A0);
  Serial.print(analogRead(A0));
  GetAnalog_Current_Fan = Get_amp - 150;
   Serial.print("\t");
  Serial.print(" Current :  ");
  Serial.print((GetAnalog_Current_Fan*0.0264193)-13);
  Serial.println(" A ");
  Analog_I_Fan = int((GetAnalog_Current_Fan*0.0264193)-13);*/
Serial.print(" Current Value : ");
Serial.print(analogRead(A0));
Serial.print(" \t ");
Serial.print(" Current RMS : ");
  get_CurrentRMSR();
  delay(2000);
  Tell_Status_Work();

  //-----------------------------------------------///
  //*****READ Condacer MIC****//*//****************************////--------------------- Function Number 4 ---------------------------

  digitalWrite(Relay_1, HIGH);
  digitalWrite(Relay_2, HIGH);
  digitalWrite(Relay_3, HIGH);
  delay(100);
  // Serial.print(analogRead(A0));
   Analog_Noise = analogRead(A0);
   Serial.print(" Noise Value : ");
   Serial.print(Analog_Noise);
   Serial.print("\t");
     if(Analog_Noise>=600) Noise_LV = 2;
     else if(Analog_Noise<600 && Analog_Noise>300) Noise_LV = 1;
     else if(Analog_Noise<=300 && Analog_Noise>=0) Noise_LV = 0;
  Serial.print(" Noise Level : ");
  Serial.println(Noise_LV);
  delay(2000);
  Tell_Status_Work();

}
//--------------------------Function_Read_value_voltagt_Fan------------//


void Read_Temp_Humidity_DHT22(){

 h1 = dht1.readHumidity();
 t1 = dht1.readTemperature();
 h2 = dht2.readHumidity();
 t2 = dht2.readTemperature();
 Serial.print(" T1 = ");
 Serial.print(t1);
 Serial.print(" H1 = ");
 Serial.println(h1);
 Serial.print(" T2 = ");
 Serial.print(t2);
 Serial.print(" H2 = ");
 Serial.println(h2);
 delay(500);
 Tell_Status_Work();
}



//--------------------------------Function_upload_Thingspeak------------------//

void upload_Thingspeak(){

  delay(2000);
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  /*--------------------------------------------------------------------V_Fan-------------------I_Fan-------------------I_Com--------------------Noise--------------------t1----------------------h1----------------------t2----------------------h2-------*/

  client.print(String("GET /update?key=84XWXY1ODD3G51TO&field1=" + String(Analog_V_Fan)+"&field2=" + String(AmpsRMS)+"&field3=" + String(Irms)+"&field4=" + String(Noise_LV)+"&field5=" + String(t1)+"&field6=" + String(h1)+"&field5=" + String(t2)+"&field6=" + String(h2)) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

}

//--------------------------------Function_work_for_Arduino--------------//
void loop() {

  Tell_Status_Work();
  Read_Temp_Humidity_DHT22();
  Function_Work();
  upload_Thingspeak();

}

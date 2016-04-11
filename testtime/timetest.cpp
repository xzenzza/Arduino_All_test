#include <ESP8266WiFi.h>
#define SERVER_PORT 80          //กำหนดใช้ Port 80
 
const char* ssid = "stk";               //กำหนด SSID
const char* password = "stk123456";     //กำหนด Password
 
const char* server_ip = "api.openweathermap.org";   //กำหนดชื่อ Server ที่ต้องการเชื่อมต่อ
String city = "Rayong"; // Bangkok,Nonthaburi       //กำหนดชื่อจังหวัดที่ต้องการขอข้อมูล
/* กำหนดค่าคำสั่ง HTTP GET */
String str_get1  = "GET /data/2.5/weather?q=";      
String str_get2  = "&mode=xml&units=metric HTTP/1.1\r\n";
String str_host = "Host: api.openweathermap.org\r\n\r\n";
 
 
 
WiFiServer server(SERVER_PORT);     //เปิดใช้งาน TCP Port 80
WiFiClient client;              //ประกาศใช้  client 
 
unsigned long previousMillis = 0;       //กำหนดตัวแปรเก็บค่า เวลาสุดท้ายที่ทำงาน    
const long interval = 10000;            //กำหนดค่าตัวแปร ให้ทำงานทุกๆ 10 วินาที
 
void setup() 
{
    Serial.begin(115200);
    WiFi.begin(ssid, password);         //เชื่อมต่อกับ AP
   
     while (WiFi.status() != WL_CONNECTED)  //ตรวจเช็ค และ รอจนเชื่อมต่อ AP สำเร็จ
    {
            delay(500);
            Serial.print(".");
    }
   
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");         
    Serial.println(WiFi.localIP());         //แสดง IP Address ที่ได้   
}
 
void loop() 
{
    while(client.available())               //ตรวจเช็คว่ามีการส่งค่ากลับมาจาก Server หรือไม่
    {
          String line = client.readStringUntil('\n');       //อ่านค่าที่ Server ตอบหลับมาทีละบรรทัด
          String get_data = cut_string(line,"city","name"); //ตัด string ข้อมูลส่วนชื่อจังหวัด
          if(get_data != "NULL")            
          {
            Serial.println("City");
            Serial.println(get_data);
          }
          get_data = cut_string(line,"temperature","value");    //ตัด string ข้อมูลส่วน อุณหภูมิ
          if(get_data != "NULL")
          {
             Serial.println("Temperature");
             Serial.println(get_data);
          }
 
          get_data = cut_string(line,"humidity","value");       //ตัด string ข้อมูลส่วน ความชื้น
          if(get_data != "NULL")
          {
              Serial.println("Humidity");
              Serial.println(get_data);
          }
          get_data = cut_string(line,"pressure","value");       //ตัด string ข้อมูลส่วน ความดัน
          if(get_data != "NULL")
          {
              Serial.println("Pressure");
              Serial.println(get_data);
          }
          
         // Serial.print(line);
    }
  unsigned long currentMillis = millis();           //อ่านค่าเวลาที่ ESP เริ่มทำงานจนถึงเวลาปัจจุบัน
  if(currentMillis - previousMillis >= interval)         /*ถ้าหากเวลาปัจจุบันลบกับเวลาก่อหน้านี้ มีค่า
                            มากกว่าค่า interval ให้คำสั่งภายใน if ทำงาน*/
  {
    previousMillis = currentMillis;              /*ให้เวลาปัจจุบัน เท่ากับ เวลาก่อนหน้าเพื่อใช้
                            คำนวณเงื่อนไขในรอบถัดไป*/
    Client_Request();
  }   
    
}
void Client_Request()
{
    Serial.println("Connect TCP Server");
    int cnt=0;
    while (!client.connect(server_ip,SERVER_PORT))  //เชื่อมต่อกับ Server และรอจนกว่าเชื่อมต่อสำเร็จ
    {
          Serial.print(".");
          delay(100);
          cnt++;
          if(cnt>50)                 //ถ้าหากใช้เวลาเชื่อมต่อเกิน 5 วินาที ให้ออกจาก ฟังก์ชั่น
          return;
    } 
    Serial.println("Success");
    delay(500);
   client.print(str_get1+city+str_get2+str_host);       //ส่งคำสั่ง HTTP GET ไปยัง Server
   Serial.print(str_get1+city+str_get2+str_host);
   delay(100);
}
 
String cut_string(String input,String header,String get_string)
{
    if (input.indexOf(header) != -1)    //ตรวจสอบว่าใน input มีข้อความเหมือนใน header หรือไม่
    {
        int num_get = input.indexOf(get_string);  //หาตำแหน่งของข้อความ get_string ใน input
        if (num_get != -1)      //ตรวจสอบว่าตำแหน่งที่ได้ไม่ใช่ -1 (ไม่มีข้อความ get_string ใน input)
        {               
            int start_val = input.indexOf("\"",num_get)+1;  // หาตำแหน่งแรกของ “ 
            int stop_val = input.indexOf("\"",start_val);   // หาตำแหน่งสุดท้ายของ “
            return(input.substring(start_val,stop_val));    //ตัดเอาข้อความระหว่า “แรก และ ”สุดท้าย  
        }
        else
        {
          return("NULL");   //Return ข้อความ NULL เมื่อไม่ตรงเงื่อนไข
        }
    }
     
    return("NULL"); //Return ข้อความ NULL เมื่อไม่ตรงเงื่อนไข
}

#include <ESP8266WiFi.h>
#include <time.h>

const char* ssid = "Smart-Soul";
const char* password = "86Irit86h,8iv'";

int timezone = 7*3600;
int dst = 0;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop() {

  time_t now = time(nullptr);
 // Serial.println(ctime(&now));
   struct tm* p_tm =localtime(&now);
  Serial.println(p_tm->tm_sec);
  Serial.println(p_tm->tm_min);
  Serial.println(p_tm->tm_hour);
  delay(1000);
}

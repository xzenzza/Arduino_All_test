#include <ESP8266WiFi.h>
#include <time.h>
#include <ESP8266HTTPClient.h>

//const char* ssid = "Smart-Soul";
//const char* password = "86Irit86h,8iv'";
const char* ssid = "@RMUTI-WiFi";
const char* password = "";

int timezone = 7 * 3600;
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
  //----------------------------------Authen--------------------------///
 HTTPClient http;
       Serial.println("[HTTP] begin...\n");
        http.begin("http://afw-kkc.rmuti.ac.th/login.php?r002391040&url=http%3A%2F%2Fgo.microsoft.com%2Ffwlink%2F%3FLinkID%3D219472&clcid=0x409"); //HTTP
        Serial.println("[HTTP] POST...\n");
        int httpCode = http.POST("_u=theerasak.su&_p=t040350579&a=a&web_host=afw-kkc.rmuti.ac.th&web_host4=afw4-kkc.rmuti.ac.th&_ip4=172.26.110.148&web_host6=&_ip6=");
        if(httpCode > 0) {
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
//----------------------------------------------------------------------////

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
  Serial.println(ctime(&now));
  struct tm* p_tm = localtime(&now);
  Serial.println(p_tm->tm_sec);
  if(p_tm->tm_sec == 0)Serial.println("WOW");
  delay(1000);
}

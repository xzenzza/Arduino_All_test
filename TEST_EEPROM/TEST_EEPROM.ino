#include <EEPROM.h>
int len;
int address = 0;

String EEPROM_read(int index, int length) {
  String text = "";
  char ch = 3;

  for (int i = index; (i < (index + length)) && ch; ++i) {
    if (ch = EEPROM.read(i)) {
      text.concat(ch);
    }
  }
  return text;
}

int EEPROM_write(int index, String text) {
  for (int i = index; i < text.length() + index; ++i) {
    EEPROM.write(i, text[i - index]);
  }
  EEPROM.write(index + text.length(), 0);
  EEPROM.commit();

  return text.length() + 1;
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("BEGIN");
  EEPROM.begin(512);

 // int len = EEPROM_write(address, "HELLO");
//  Serial.print("SAVED .. ");
//  Serial.println(len);
  Serial.println(EEPROM_read(address, 5));  
}

void loop()
{
   
}

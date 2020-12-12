/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    08.12.2020
    @see The GNU Public License (GPL) Version 3
*/
#ifdef ESP32
  #include <HardwareSerial.h>
  HardwareSerial lcd(2);
#endif
#ifdef ESP8266
  #include <SoftwareSerial.h>
  SoftwareSerial lcd;
#endif

#include "Nextion.h" // If using ESP8266, line 37 in NexConfig.h needs to be changed "#define nexSerial Serial2" to "#define nexSerial Serial1"

void init_LCD()
{

  #ifdef ESP32
    #define RXD2 16 // LCD
    #define TXD2 17 // LCD
    lcd.begin(9600, SERIAL_8N1, RXD2, TXD2);
  #endif
  #ifdef ESP8266
    lcd.begin(9600, SWSERIAL_8N1, 14, 12, false, 95, 25);
  #endif
  
  /*LCD.print("baud=115200");
  endNextionCommand();
  LCD.end();
  LCD.begin(115200);*/
}

/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include "Nextion.h"
#include <HardwareSerial.h>
HardwareSerial lcd(2);

void init_LCD()
{

  #define RXD2 16 // LCD
  #define TXD2 17 // LCD
  
  lcd.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  /*LCD.print("baud=115200");
  endNextionCommand();
  LCD.end();
  LCD.begin(115200);*/
}

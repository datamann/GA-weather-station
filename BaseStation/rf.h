/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    01.12.2020
    @see The GNU Public License (GPL) Version 3
*/

#include <RH_ASK.h>

#ifdef ESP32
  RH_ASK rf(2000, 4, -0, -0);
#endif

#ifdef ESP8266
  RH_ASK rf(2000, 2, -0, -0);
#endif


bool init_rf() {
  bool status = false;
  status = rf.init();
}

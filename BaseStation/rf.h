/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include <RH_ASK.h>

RH_ASK rf(2000, 4, -0, -0);

bool init_rf() {
  bool status = false;
  status = rf.init();
}

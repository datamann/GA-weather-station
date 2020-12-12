/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include <time.h>
#include "DisplayData.h"

const char* NTP_SERVER = "ch.pool.ntp.org";
// enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
const char* TZ_INFO = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";

// Structs
tm timeinfo;
time_t now;

long unsigned lastNTPtime;

void init_ntp() {
  configTime(0, 0, NTP_SERVER);
  // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
  setenv("TZ", TZ_INFO, 1);
}

void getTime() {
  
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  // Struct "td" is defined in config.h
  strftime (td.customTime,6,"%H:%M",timeinfo);
  strftime (td.customHour,3,"%H",timeinfo);
  strftime (td.customDay,10,"%A",timeinfo);
  strftime (td.customDate,9,"%d.%m.%y",timeinfo);
}

bool getNTPtime(int sec) {

  {
    uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &timeinfo);
      delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    if (timeinfo.tm_year <= (2016 - 1900)) return false;  // the NTP call was not successful
    
    char time_output[30];
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now));
  }
  return true;
}

void showTime(tm localTime) {
  #ifdef DEBUG
    Serial.print(localTime.tm_mday);
    Serial.print('/');
    Serial.print(localTime.tm_mon + 1);
    Serial.print('/');
    Serial.print(localTime.tm_year - 100);
    Serial.print('-');
    Serial.print(localTime.tm_hour);
    Serial.print(':');
    Serial.print(localTime.tm_min);
    Serial.print(':');
    Serial.print(localTime.tm_sec);
    Serial.print(" Day of Week ");
  #endif
  if (localTime.tm_wday == 0)   Serial.println(7);
  else Serial.println(localTime.tm_wday);
}

/*void getTime() {
  
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (customTime,6,"%H:%M",timeinfo);
  strftime (customDay,9,"%A",timeinfo);
  strftime (customDate,9,"%d.%m.%y",timeinfo);
}*/

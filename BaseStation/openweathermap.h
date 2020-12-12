/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    07.12.2020
    @see The GNU Public License (GPL) Version 3
*/

#ifndef OPENWEATHERMAP_h
#define OPENWEATHERMAP_h
#include "WeatherData.h"

#ifdef ESP32
  #include <WiFi.h>
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
#endif

#if (ARDUINO >= 100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class OpenWeatherMap {
  public:
    OpenWeatherMap();
    WeatherData fetchWeatherData(String cityid, String apikey, WeatherData *ptr);
  
  private:
    const char* getWindDirection(int deg);
    bool inRange(float val, float minimum, float maximum);
    char* _servername = "api.openweathermap.org";
    
    #ifdef ESP8266
      String httpGETRequest(const char* serverName);
      WiFiClient wifiConnect(String cityid, String apikey);
    #endif
};
#endif

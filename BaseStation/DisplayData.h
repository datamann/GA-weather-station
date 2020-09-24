/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#ifndef DISPLAYDATA_h
#define DISPLAYDATA_h

#if (ARDUINO >= 100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "WeatherData.h"
#include "SensorData.h"

/*
  Screens 0/1/2
  Text:
  description
  temperature
  tempfeelslike
  tempmax
  tempmin
  humidity
  pressure
  windspeed
  winddirection
  location
  ipaddress
  
  Images:
  weathericon
  radioactive
  radioinactive
*/
class DisplayData {
  public:
    DisplayData();
    void endNextionCommand();
    void turnPage(String* cityid, String pageid, char* Time, char* Day, char* Date, WeatherData City1, WeatherData City2, SensorData sensorData, String IP);
    void showConnectingIcon(String pageid, String imageid);
    void showicon(String icon, int action);
    void printToLCD(String field, String data);
    void updateTimeLCD(String Time);
    void updateDateLCD(String Day, String Date);
    const char* getWeatherIcon(int id, String pageid);
    void readLCDData(String* cityid, String Day, String Date, String Time, WeatherData City1, WeatherData City2, SensorData sensorData, String IP);
  
  private:
    void drawFog(String pageid);
    void drawHeavySnowfall(String pageid);
    void drawModerateSnowfall(String pageid);
    void drawLightSnowfall(String pageid);
    void drawHeavyRain(String pageid);
    void drawModerateRain(String pageid);
    void drawLightRain(String pageid);
    void drawLightRainWithSunOrMoon(String pageid);
    void drawThunderstorm(String pageid);
    void drawClearWeather(String pageid);
    void drawCloud(String pageid);
    void drawFewClouds(String pageid);
    //void printWeatherIcon(int id, String pageid);
    void printWeatherData(String cityid, WeatherData City, String IP);
};
#endif

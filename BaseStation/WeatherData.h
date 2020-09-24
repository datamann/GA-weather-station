/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#ifndef WEATHERDATA_h
#define WEATHERDATA_h

#if (ARDUINO >= 100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class WeatherData {

  public:
    WeatherData();
    
    void setLocation(const char* location);
    char* getLocation();

    void setTemperature(float temperature);
    float getTemperature();

    void setFeelslike(float feelsLike);
    float getFeelslike();
    
    void setTemp_min(float temp_min);
    float getTemp_min();

    void setTemp_max(float temp_max);
    float getTemp_max();

    void setPressure(int pressure);
    int getPressure();

    void setHumidity(int humidity);
    int getHumidity();

    void setWeather(const char* weather);
    char* getWeather();

    void setDescription(const char* description);
    char* getDescription();

    void setWindspeed(float windSpeed);
    float getWindspeed();

    void setWinddirection(int windDirection);
    int getWinddirection();

    void setWindcompassdirection(const char* windCompassDirection);
    char* getWindcompassdirection();

    void setWeatherid(int weatherID);
    int getWeatherid();

    void setTimes(const char* timeS);
    char* getTimes();

  private:
    char _location[50];
    float _temperature;
    float _feelsLike;
    float _temp_min;
    float _temp_max;
    int _pressure;
    int _humidity;
    char _weather[10];
    char _description[20];
    float _windSpeed;
    int _windDirection;
    char _windCompassDirection[4];
    int _weatherID;
    char _timeS[10];
};
#endif

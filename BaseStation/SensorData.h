/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#ifndef SENSORDATA_h
#define SENSORDATA_h

#if (ARDUINO >= 100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

class SensorData {
  public:
    SensorData();
    
    void setTemperature(float temperature);
    float getTemperature();

    void setAltitude(float altitude);
    float getAltitude();

    void setPressure(int pressure);
    int getPressure();

    void setHumidity(float humidity);
    float getHumidity();

    void setBattery(float battery);
    float getBattery();

    void setSensorid(float sensorid);
    float getSensorid();

  private:
    float _temperature;
    float _altitude;       // TODO: Remove!
    int _pressure;
    float _humidity;
    float _battery;
    uint8_t _sensorid;
    //time_t _timeStamp;   // For future use
};
#endif

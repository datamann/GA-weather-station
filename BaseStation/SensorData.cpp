/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include "SensorData.h"

SensorData::SensorData(){
  _temperature = 0.00;
  _altitude = 0.00;       // TODO: Remove!
  _pressure = 0;
  _humidity = 0.00;
  _battery = 0.00;
  _sensorid = 0;
};

void SensorData::setTemperature(float temperature) {
  _temperature = temperature;
  //strncpy (_temperature, temperature, sizeof(_temperature));
};

float SensorData::getTemperature() {
  return _temperature;
};

// TODO: Remove
void SensorData::setAltitude(float altitude) {
  _altitude = altitude;
};

// TODO: Remove
float SensorData::getAltitude() {
  return _altitude;
};

void SensorData::setPressure(int pressure) {
  _pressure = pressure;
};

int SensorData::getPressure() {
  return _pressure;
};

void SensorData::setHumidity(float humidity) {
  _humidity = humidity;
};

float SensorData::getHumidity() {
  return _humidity;
};

void SensorData::setBattery(float battery) {
  _battery = battery;
};

float SensorData::getBattery() {
  return _battery;
};

void SensorData::setSensorid(float sensorid) {
  _sensorid = sensorid;
};

float SensorData::getSensorid() {
  return _sensorid;
};

/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include "WeatherData.h"

WeatherData::WeatherData() {
  _location[7] = '------';
  _temperature = 0.00;
  _feelsLike = 0.00;
  _temp_min = 0.00;
  _temp_max = 0.00;
  _pressure = 0;
  _humidity = 0;
  _windSpeed = 0.00;
  _windDirection = 0;
  _weatherID = 0;
};

void WeatherData::setLocation(const char* location) {
  strncpy (_location, location, sizeof(_location));
};

char* WeatherData::getLocation() {
  return _location;
};

void WeatherData::setTemperature(float temperature) {
  _temperature = temperature;
};

float WeatherData::getTemperature() {
  return _temperature;
};

void WeatherData::setFeelslike(float feelsLike) {
  _feelsLike = feelsLike;
};

float WeatherData::getFeelslike() {
  return _feelsLike;
};

void WeatherData::setTemp_min(float temp_min) {
  _temp_min = temp_min;
};

float WeatherData::getTemp_min() {
  return _temp_min;
};

void WeatherData::setTemp_max(float temp_max) {
  _temp_max = temp_max;
};

float WeatherData::getTemp_max() {
  return _temp_max;
};

void WeatherData::setPressure(int pressure) {
  _pressure = pressure;
};

int WeatherData::getPressure() {
  return _pressure;
};

void WeatherData::setHumidity(int humidity) {
  _humidity = humidity;
};

int WeatherData::getHumidity() {
  return _humidity;
};

void WeatherData::setWeather(const char* weather) {
  strncpy (_weather, weather, sizeof(_weather));
};

char* WeatherData::getWeather() {
  return _weather;
};

void WeatherData::setDescription(const char* description) {
  strncpy (_description, description, sizeof(_description));
};

char* WeatherData::getDescription() {
  return _description;
};

void WeatherData::setWindspeed(float windSpeed) {
  _windSpeed = windSpeed;
};

float WeatherData::getWindspeed() {
  return _windSpeed;
};

void WeatherData::setWindcompassdirection(const char* windCompassDirection) {
  strncpy (_windCompassDirection, windCompassDirection, sizeof(_windCompassDirection));
};

char* WeatherData::getWindcompassdirection() {
  return _windCompassDirection;
};

void WeatherData::setWinddirection(int windDirection) {
  _windDirection = windDirection;
};

int WeatherData::getWinddirection() {
  return _windDirection;
};

void WeatherData::setWeatherid(int weatherID) {
  _weatherID = weatherID;
};

int WeatherData::getWeatherid() {
  return _weatherID;
};

void WeatherData::setTimes(const char* timeS) {
  strncpy (_timeS, timeS, sizeof(_timeS));
};

char* WeatherData::getTimes() {
  return _timeS;
};

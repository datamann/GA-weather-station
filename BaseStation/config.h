/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include "credentials.h"
#include "wifi.h"
#include "SensorData.h"
#include "openweathermap.h"

//String RemoteSensorLocation = "Skogliveien 14"; // In use in DisplayData.cpp

// Array to stor time and date from NTP call.
struct TimeDate{
  char customTime[6];
  char customHour[3];
  char customDay[10];
  char customDate[9];
} td;

// RF data strucure
struct rfwd {
    float temperature;
    float altitude;       // TODO: Remove!
    float pressure;
    float humidity;
    float battery;
    uint8_t sensorID;
    //time_t timeStamp;   // For future use
}; rfwd weatherdata;

/*
 * Written by Stig B. Sivertsen
 * sbsivertsen@gmail.com
 * https://github.com/datamann/GA-weather-station
 * 18.01.2020
 * @see The GNU Public License (GPL) Version 3
*/
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf(2000, 2, 4, 5);

#define DEBUG

struct wd {
    float temperature;
    float altitude;
    float pressure;
    float humidity;
    //float battery;      // For future use
    //uint8_t  sensorID;  // For future use
    //time_t timeStamp;   // For future use
};
wd weatherdata;

void setup() {
  
    Serial.begin(9600);
    
    if (!rf.init())
    {
        #ifdef DEBUG
            Serial.println(F("RF init failed!"));
        #endif
    }
}

void loop() {

    //uint8_t buf[];
    //uint8_t buflen = sizeof(buf);

    uint8_t data[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(data);
    
    if (rf.recv(data, &len)) // Non-blocking
    {
        wd* weatherdata = (wd*)data;

        #ifdef DEBUG
            char buf[21];
  
            char temperature[6];
            char pressure[5];
            char altitude[6];
            char humidity[5];
      
            dtostrf(weatherdata->temperature, 3, 2, temperature);
            dtostrf(weatherdata->pressure, 3, 0, pressure);
            dtostrf(weatherdata->altitude, 3, 2, altitude);
            dtostrf(weatherdata->humidity, 3, 0, humidity);
      
            sprintf(buf,"%s %s %s %s", temperature, pressure, altitude, humidity);
            Serial.print(F("Buf: "));
            Serial.println(buf);
        #endif
    }
}

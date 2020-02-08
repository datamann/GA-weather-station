/*
 * Written by Stig B. Sivertsen
 * sbsivertsen@gmail.com
 * https://github.com/datamann/GA-weather-station
 * 18.01.2020
 * @see The GNU Public License (GPL) Version 3
*/
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf(500, 2, 0, 5); // 500 bps, recieve on GPIO2, transmit on GPIO0 (Recieve only), PTT on GPIO5.

#define DEBUG

struct wd {
    float temperature;
    float altitude;
    float pressure;
    float humidity;
    float battery;
    uint8_t  sensorID;
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
            char buf[31];
  
            char temperature[7];
            char pressure[6];
            char altitude[5];
            char humidity[6];
            char batt[5];
            char id[2];
      
            dtostrf(weatherdata->temperature, 3, 2, temperature);
            dtostrf(weatherdata->pressure, 4, 0, pressure);
            dtostrf(weatherdata->altitude, 4, 0, altitude);
            dtostrf(weatherdata->humidity, 3, 0, humidity);
            dtostrf(weatherdata->battery, 1, 2, batt);
            dtostrf(weatherdata->sensorID, 1, 0, id);

            Serial.print(F("Temperature: "));
            Serial.print(temperature);
            Serial.println(F("c"));
            
            Serial.print(F("Pressure: "));
            Serial.print(pressure);
            Serial.println(F("hpa"));
            
            Serial.print(F("Altitude: "));
            Serial.print(altitude);
            Serial.println(F("m"));
            
            Serial.print(F("Humidity: "));
            Serial.print(humidity);
            Serial.println(F("%"));
            
            Serial.print(F("Battery: "));
            Serial.print(batt);
            Serial.println(F("v"));
            
            Serial.print(F("Sensor ID: "));
            Serial.println(id);
            
            Serial.println();
            
            //sprintf(buf,"%s %s %s %s %s %s", temperature, pressure, altitude, humidity, batt, id);
            //Serial.print(F("Buf: "));
            //Serial.println(buf);
        #endif
    }
}

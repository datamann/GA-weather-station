/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    20.04.2020
    @see The GNU Public License (GPL) Version 3
*/
/*

// Concatination
char URI[256] = "";
strcat(URI, API_URI);
strcat(URI, API_KEY);

*/
#include <RH_ASK.h>
#include <SPI.h>

RH_ASK rf(2000, 10, -0, -0);

#define DEBUG

struct wd {
    float temperature;
    float altitude;
    float pressure;
    float humidity;
    float battery;
    uint8_t sensorID;
    //time_t timeStamp;   // For future use
}; wd weatherdata;

void setup() {
    
    Serial.begin(9600);

    if (!rf.init())
    {
      #ifdef DEBUG
        Serial.println(F("RF init failed!"));
      #endif
    } else {
      #ifdef DEBUG
        Serial.println(F("RF init succeeded!"));
      #endif
    }
}

unsigned long connection_lost;
const unsigned long wait_for_connection = 30000; // 30 sec in ms
bool timerSet = false;

void loop() {
    
    uint8_t data[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(data);

    unsigned long now = millis();

    now = millis();
    if (!rf.available())
    {
      if (!timerSet)
      {
        connection_lost = now;
      }
      timerSet = true;
    }
    else
    {
      timerSet = false;
      
      if (rf.recv(data, &len)) // Non-blocking
      {
          wd* weatherdata = (wd*)data;
  
          #ifdef DEBUG
            //char buf[31];
            
            char temperature[7];
            char pressure[6];
            char altitude[5];
            char humidity[6];
            char batt[5];
            char id[2];
            
            dtostrf(weatherdata->temperature, 3, 1, temperature);
            dtostrf(weatherdata->pressure, 4, 0, pressure);
            dtostrf(weatherdata->altitude, 4, 0, altitude);
            dtostrf(weatherdata->humidity, 3, 0, humidity);
            dtostrf(weatherdata->battery, 1, 2, batt);
            dtostrf(weatherdata->sensorID, 1, 0, id);

            Serial.println(F("Basestation: "));
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
          #endif
      }
    }
}

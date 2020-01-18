/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

 /*
 * Written by Stig B. Sivertsen
 * sbsivertsen@gmail.com
 * https://github.com/datamann/GA-weather-station
 * 18.01.2020
 * @see The GNU Public License (GPL) Version 3
*/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <RH_ASK.h>

#define SEALEVELPRESSURE_HPA (1013.25)
#define DEBUG

Adafruit_BME280 bme;
RH_ASK rf;

unsigned long delayTime;

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
    
    while(!Serial);
        #ifdef DEBUG
            Serial.println(F("BME280 running"));
        #endif

    unsigned status;
    status = bme.begin(0x76);
    
    if (!status) {
        #ifdef DEBUG
            Serial.println(F("Could not find a valid BME280 sensor, check wiring, address, sensor ID!"));
            Serial.print(F("SensorID was: 0x"));
            Serial.println(bme.sensorID(),16);
            Serial.print(F("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n"));
            while (1) delay(10);
        #endif
    }

    delayTime = 1000;
    if (!rf.init())
    {
        #ifdef DEBUG
            Serial.println(F("RF init failed"));
        #endif
    }
}


void loop() { 
    printValues();
    delay(delayTime);

    rf.send((uint8_t *)&weatherdata, sizeof(weatherdata));
    rf.waitPacketSent();
    delay(1000);
}


void printValues() {

    weatherdata.temperature = bme.readTemperature();
    weatherdata.pressure = bme.readPressure() / 100.0F;
    weatherdata.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    weatherdata.humidity = bme.readHumidity();

    #ifdef DEBUG
        char buf[21];
      
        char temperature[6];
        char pressure[5];
        char altitude[6];
        char humidity[5];
      
        dtostrf(weatherdata.temperature, 3, 2, temperature);
        dtostrf(weatherdata.pressure, 3, 0, pressure);
        dtostrf(weatherdata.altitude, 3, 2, altitude);
        dtostrf(weatherdata.humidity, 3, 0, humidity);
      
        sprintf(buf,"%s %s %s %s", temperature, pressure, altitude, humidity);
        Serial.print(F("Buf: "));
        Serial.println(buf);
    #endif
}

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
 * 06.02.2020
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

// Default RX=D11,TX=D12
//RH_ASK rf;
RH_ASK rf(500, 0,11);

// ATTiny
//RH_ASK driver(2000, 4, 3);

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
int PROBE = 2;
void setup() {
    analogReference(EXTERNAL);
    pinMode(PROBE, OUTPUT);
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

unsigned long target_time = 0L;
const unsigned long PERIOD = 1*05*1000UL;

void loop() {

    if (millis() - target_time >= PERIOD)
    {
        turnOn();
        readVoltage();
        printValues();
        sendPacket();
        target_time += PERIOD;
    }
}

void sendPacket(){

    rf.send((uint8_t *)&weatherdata, sizeof(weatherdata));
    rf.waitPacketSent();
    rf.setModeIdle();
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

void readVoltage(){
  voltsIn = (unsigned) analogRead(A1);
  // 3.60/1023 = 0.003519061583578
  volt = voltsIn * 0.003538611925709;
}

void turnOn(){
  digitalWrite(2, HIGH);
  Serial.println(F("Turned On..."));
}
void turnOff(){
  digitalWrite(2, LOW);
  Serial.println(F("Turned Off..."));
}

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

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <RH_ASK.h>

int nbr_remaining;
int sleep_time = 1;

ISR(WDT_vect)
{
  wdt_reset();
}

// Comment out to turn off debug
#define DEBUG

#define SEALEVELPRESSURE_HPA (1013.25) // Default sea level pressure

Adafruit_BME280 bme;

RH_ASK rf(2000, -0,12); // 500bps, RX=0, TX=D11 - Transmitt only

unsigned long delayTime;

struct wd {
  float temperature;
  float altitude;
  float pressure;
  float humidity;
  float battery;
  uint8_t sensorID = 1;
  //time_t timeStamp;   // For future use
}; wd weatherdata;

int PROBESWITCH = 2;  // Pin D2
int PROBE = 1;       // Pin A1
int SWITCH;

void configure_wdt(void) {
  cli();
  MCUSR = 0;
  WDTCSR |= 0b00011000;
  WDTCSR =  0b01000000 | 0b100001;
  sei();
}

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{  
  nbr_remaining = ncycles;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_adc_disable();
 
  while (nbr_remaining > 0) {
    sleep_mode();
    sleep_disable();
    nbr_remaining = nbr_remaining - 1;
  }
  power_all_enable();
}

void setup() {
    configure_wdt();
    analogReference(EXTERNAL);
    
    pinMode(PROBESWITCH, OUTPUT);
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

void turnOnProbe(){
  digitalWrite(PROBESWITCH, HIGH);
  #ifdef DEBUG
    Serial.println(F("Turned probe on..."));
  #endif
  delay(50);
}
void turnOffProbe(){
  digitalWrite(PROBESWITCH, LOW);
  #ifdef DEBUG
    Serial.println(F("Turned probe off..."));
  #endif
  delay(50);
}

void turnOnBME280(){
    bme.setSampling(Adafruit_BME280::MODE_NORMAL);
    delay(50);
}

void turnOffBME280(){
    bme.setSampling(Adafruit_BME280::MODE_SLEEP);
    delay(50);
}

float volt;
float voltsIn;
void readVoltage(){
  volt = 0.00;
  voltsIn = (unsigned) analogRead(PROBE);
  volt = voltsIn * 0.003538611925709;   // 3.60/1023 = 0.003519061583578
}

void sendPacket(){
    rf.send((uint8_t *)&weatherdata, sizeof(weatherdata));
    rf.waitPacketSent();
}

void readWeatherData(){
    
    weatherdata.temperature = 0;
    weatherdata.pressure = 0;
    weatherdata.altitude = 0;
    weatherdata.humidity = 0;
    weatherdata.battery = 0;
    
    weatherdata.temperature = bme.readTemperature();
    weatherdata.pressure = bme.readPressure() / 100.0F;
    weatherdata.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    weatherdata.humidity = bme.readHumidity();
    weatherdata.battery = volt;
}

void loop() {

    Serial.println(F("Waking up..."));
    
    // Turn on all sensors and read data
    turnOnBME280();
    readWeatherData();
    turnOnProbe();
    readVoltage();
    printValues();
    sendPacket();

    // Turn off all sensors to save power
    turnOffBME280();
    turnOffProbe();

    Serial.println(F("Going to sleep..."));
    delay(20);
    sleep(sleep_time);
}

void printValues() {

    #ifdef DEBUG
        char buf[31];
      
        char temperature[7];
        char pressure[6];
        char altitude[5];
        char humidity[6];
        char batt[5];
        char id[2];
      
        dtostrf(weatherdata.temperature, 3, 2, temperature);
        dtostrf(weatherdata.pressure, 4, 0, pressure);
        dtostrf(weatherdata.altitude, 4, 0, altitude);
        dtostrf(weatherdata.humidity, 3, 0, humidity);
        dtostrf(weatherdata.battery, 1, 2, batt);
        dtostrf(weatherdata.sensorID, 1, 0, id);
      
        sprintf(buf,"%s %s %s %s %s %s", temperature, pressure, altitude, humidity, batt, id);
        Serial.print(F("Buf: "));
        Serial.println(buf);
    #endif
}

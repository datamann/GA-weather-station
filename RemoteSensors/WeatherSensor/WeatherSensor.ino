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
 * 21.09.2020
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
int sleep_time = 19; // 19 = 2.5min

ISR(WDT_vect)
{
  wdt_reset();
}

// Comment out to turn off debug
//#define DEBUG

#define SEALEVELPRESSURE_HPA (1013.25) // Default sea level pressure

Adafruit_BME280 bme;
RH_ASK rf(2000, -0,10); // 2000bps, RX=0, TX=D10 - Transmitt only

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

void configure_wdt(void) {
  cli();
  MCUSR = 0;
  WDTCSR |= 0b00011000;
  WDTCSR =  0b01000000 | 0b100001;
  sei();

  /*cli();
  WDTCSR = (24);//change enable and WDE - also resets
  WDTCSR = (33);//prescalers only - get rid of the WDE and WDCE bit
  WDTCSR |= (1<<6);//enable interrupt mode
  sei();*/
}

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{  
  nbr_remaining = ncycles;
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  //power_adc_disable();
  ADCSRA &= ~(1 << 7);
 
  while (nbr_remaining > 0) {
    sleep_mode();
    sleep_disable();
    nbr_remaining = nbr_remaining - 1;
  }
  power_all_enable();
}

void setup() {
    configure_wdt();

    for (int i = 0; i < 20; i++) {
      pinMode(i, OUTPUT);
    }
    
    analogReference(INTERNAL);
    analogRead(A0);
    
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

  // Blinking LED to indicate that the Remote Sensor is alive.
  for (int i = 0; i < 6; i++) {
    digitalWrite(2, HIGH);
    delay(50);
    digitalWrite(2, LOW);
    delay(50);
  }
}

void turnOnBME280(){
    bme.setSampling(Adafruit_BME280::MODE_NORMAL);
    delay(50);
}

void turnOffBME280(){
    bme.setSampling(Adafruit_BME280::MODE_SLEEP);
    delay(50);
}

float getBatteryVolts() {
  // http://www.gammon.com.au/adc
  // https://github.com/RalphBacon/Arduino-Battery-Monitor

  // You MUST measure the voltage at pin 21 (AREF) using just a simple one line sketch consisting
  // of:  analogReference(INTERNAL);
  //      analogRead(A0);
  // Then use the measured value here.

  const float InternalReferenceVoltage = 1.096; // <- as measured (or 1v1 by default)

  // turn ADC on
  ADCSRA =  bit (ADEN);

  // Prescaler of 128
  ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);

  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG) - Selects channel 14, bandgap voltage, to measure
  ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);

  // let it stabilize
  delay (50);

  // start a conversion
  bitSet (ADCSRA, ADSC);

  // Wait for the conversion to finish
  while (bit_is_set(ADCSRA, ADSC))
  {
    ;
  }

  // Float normally reduces precion but works OK here. Add 0.5 for rounding not truncating.
  float results = InternalReferenceVoltage / float (ADC + 0.5) * 1024.0;
  ADMUX =~ bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
  return results;
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
}

void loop() {

    Serial.println(F("Waking up..."));
    
    turnOnBME280();
    readWeatherData();
    turnOffBME280();
    weatherdata.battery = getBatteryVolts();
    printValues();
    sendPacket();

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

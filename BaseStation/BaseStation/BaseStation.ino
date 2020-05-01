/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    30.04.2020
    @see The GNU Public License (GPL) Version 3
*/
/*

// Concatination
char URI[256] = "";
strcat(URI, API_URI);
strcat(URI, API_KEY);

*/
#include <WiFi.h>
#include <RH_ASK.h>
#include <SPI.h>
#include "Nextion.h"
#include <time.h>
#include "wificredentials.h"
#include <HardwareSerial.h>
HardwareSerial LCD(2);

//char ssid[]           = "YOUR_SSID";
//const char* password  = "***********";

RH_ASK rf(2000, 4, -0, -0);

#define DEBUG
#define RXD2 16 // Serial2
#define TXD2 17 // Serial2

const char* NTP_SERVER = "ch.pool.ntp.org";
// enter your time zone (https://remotemonitoringsystems.ca/time-zone-abbreviations.php)
const char* TZ_INFO    = "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00";

// Structs
tm timeinfo;
time_t now;

long unsigned lastNTPtime;
unsigned long lastEntryTime;
unsigned long connection_lost;
const unsigned long wait_for_connection = 30000; // 30 sec in ms
bool timerSet = false;

bool timeChanged = true;
String compareTime;
bool dateChanged = true;
String compareDate;

char customTime[6];
char customDay[9];
char customDate[9];

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
    LCD.begin(9600, SERIAL_8N1, RXD2, TXD2);

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

    WiFi.begin(ssid, password);
    
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      if (++counter > 200) ESP.restart();
      Serial.print ( "." );
    }
    Serial.println("\n\nWiFi connected\n\n");

    configTime(0, 0, NTP_SERVER);
    // See https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv for Timezone codes for your region
    setenv("TZ", TZ_INFO, 1);

    getNTPtime(10);
    showTime(timeinfo);
    lastNTPtime = time(&now);
    lastEntryTime = millis();
    getTime();
    updateTimeLCD();
    updateDateLCD();
}
  
void getTime() {
  
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (customTime,6,"%H:%M",timeinfo);
  strftime (customDay,9,"%A",timeinfo);
  strftime (customDate,9,"%d.%m.%y",timeinfo);
}

void updateTimeLCD() {

    // Hours and minutes
    LCD.print("txtTime.txt=");
    LCD.print("\"");
    LCD.print(customTime);
    LCD.print("\"");
    LCD.write(0xff);
    LCD.write(0xff);
    LCD.write(0xff);
}

void updateDateLCD() {

    // Day
    LCD.print("txtDay.txt=");
    LCD.print("\"");
    LCD.print(customDay);
    LCD.print("\"");
    LCD.write(0xff);
    LCD.write(0xff);
    LCD.write(0xff);
  
    // Date
    LCD.print("txtDate.txt=");
    LCD.print("\"");
    LCD.print(customDate);
    LCD.print("\"");
    LCD.write(0xff);
    LCD.write(0xff);
    LCD.write(0xff);
}

void loop() {
    
    getNTPtime(1000);
    getTime();

    String oldTime = String(customTime);
    if (timeChanged)
    {
      compareTime = oldTime;
      timeChanged = false;
    }

    if (compareTime != oldTime)
    {
      updateTimeLCD();
      #ifdef DEBUG
        Serial.print(F("Time: "));
        Serial.println(customTime);
      #endif
      
      timeChanged = true;
    }

    String oldDate = String(customDate);
    if (dateChanged)
    {
      compareDate = oldDate;
      dateChanged = false;
    }

    if (compareDate != oldDate)
    {
      updateDateLCD();
      #ifdef DEBUG
        Serial.print(F("Day: "));
        Serial.println(customDay);
        Serial.print(F("Date: "));
        Serial.println(customDate);
      #endif
      
      dateChanged = true;
    }

    unsigned long now = millis();
    if (now - connection_lost >= wait_for_connection)
    {
        // Shows LCD lost connecton warning label
        LCD.print("vis txtWarning,1");
        LCD.write(0xff);
        LCD.write(0xff);
        LCD.write(0xff);
    }
    else
    {
        // Hides LCD lost connecton warning label
        LCD.print("vis txtWarning,0");
        LCD.write(0xff);
        LCD.write(0xff);
        LCD.write(0xff);
    }

    uint8_t data[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(data);

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

            // Hides LCD lost connecton warning label
            LCD.print("vis txtWarning,0");
            LCD.write(0xff);
            LCD.write(0xff);
            LCD.write(0xff);
        
            // Temperature
            LCD.print("txtTempOut.txt=");
            LCD.print("\"");
            LCD.print(temperature);
            LCD.print("\"");
            LCD.write(0xff);
            LCD.write(0xff);
            LCD.write(0xff);
        
            // Humidity
            LCD.print("txtHumid.txt=");
            LCD.print("\"");
            LCD.print(humidity);
            LCD.print("\"");
            LCD.write(0xff);
            LCD.write(0xff);
            LCD.write(0xff);
        
            // Altitude
            LCD.print("txtAlt.txt=");
            LCD.print("\"");
            LCD.print(altitude);
            LCD.print("\"");
            LCD.write(0xff);
            LCD.write(0xff);
            LCD.write(0xff);
        
            // Pressure
            LCD.print("txtPressure.txt=");
            LCD.print("\"");
            LCD.print(pressure);
            LCD.print("\"");
            LCD.write(0xff);
            LCD.write(0xff);
            LCD.write(0xff);
        
            // Battery
            LCD.print("txtBatt.txt=");
            LCD.print("\"");
            LCD.print(batt);
            LCD.print("\"");
            LCD.write(0xff);
            LCD.write(0xff);
            LCD.write(0xff);
      }
    }
}

bool getNTPtime(int sec) {

  {
    uint32_t start = millis();
    do {
      time(&now);
      localtime_r(&now, &timeinfo);
      delay(10);
    } while (((millis() - start) <= (1000 * sec)) && (timeinfo.tm_year < (2016 - 1900)));
    if (timeinfo.tm_year <= (2016 - 1900)) return false;  // the NTP call was not successful
    
    char time_output[30];
    strftime(time_output, 30, "%a  %d-%m-%y %T", localtime(&now));
  }
  return true;
}

void showTime(tm localTime) {
  Serial.print(localTime.tm_mday);
  Serial.print('/');
  Serial.print(localTime.tm_mon + 1);
  Serial.print('/');
  Serial.print(localTime.tm_year - 100);
  Serial.print('-');
  Serial.print(localTime.tm_hour);
  Serial.print(':');
  Serial.print(localTime.tm_min);
  Serial.print(':');
  Serial.print(localTime.tm_sec);
  Serial.print(" Day of Week ");
  if (localTime.tm_wday == 0)   Serial.println(7);
  else Serial.println(localTime.tm_wday);
}

/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    11.07.2021
    @see The GNU Public License (GPL) Version 3
*/

#include <SPI.h>
#include "config.h"

#ifdef ESP32
  #include "esp_bt.h"
#endif

#define DEBUG

// Cities to fetch weather data from
String cityid[] = {"3159016","1711146"}; // Drammen, NO - Ilagan, PH

#include "DisplayData.h"
#include "ntp.h"
#include "rf.h"
#include <ArduinoMqttClient.h>

// Instance of Open Weather Map class
OpenWeatherMap owpDrammen;
OpenWeatherMap owpIlagan;

// Instance of Weather data class
WeatherData ilg;
WeatherData drm;
SensorData sensorData;

// Instance of Display data class
DisplayData dp;


// MQTT Connection
WiFiClient client = init_Wifi();

MqttClient mqttClient(client); // Keepalive is changed in source file (constructor) from 60 to 90 sec.
const char broker[] = "192.168.1.50";
int port = 1883;
const char topicTemperature[]  = "remotesensor/temperature";
const char topicHumidity[]  = "remotesensor/humidity";
const char topicPressure[]  = "remotesensor/pressure";
const char topicBattery[]  = "remotesensor/battery";


void setup() {

  Serial.begin(115200);

  bool rf_connected = init_rf();
  if (!rf_connected) {
    dp.showicon("radioinactive", 1);
    #ifdef DEBUG
      Serial.println();
      Serial.println(F("RF init failed!"));
    #endif
  } else {
    
    dp.showicon("radioinactive", 0);
    dp.showicon("radioactive", 1);
    
    #ifdef DEBUG
      Serial.println(F("RF init succeeded!"));
    #endif
  }

  
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    dp.showicon("wifiicon", 1);
    delay(200);
    if (++counter > 200) ESP.restart();
    #ifdef DEBUG
      Serial.print (".");
    #endif
  }

  #ifdef ESP32
    // Disable WIFI sleep mode
    if(WiFi.getSleep() == true) {
      WiFi.setSleep(false);
      #ifdef DEBUG
        Serial.println("WiFi Sleep is now deactivated.");
      #endif
    }
  #endif
  
  #ifdef DEBUG
    Serial.print(F("\n\nWiFi connected: "));
    Serial.println(WiFi.localIP().toString());
  #endif

  // If wifi is not connected, show no connection icon
  dp.showicon("wifiicon", 0);

  // You can provide a username and password for authentication
  //mqttClient.setUsernamePassword("sbsivertsen", "j2x83vkf3EDu3W22647e");

  #ifdef DEBUG
    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);
  #endif

  if (!mqttClient.connect(broker, port)) {
    #ifdef DEBUG
      Serial.print("MQTT connection failed! Error code = ");
      Serial.println(mqttClient.connectError());
    #endif
  } else {
    #ifdef DEBUG
      Serial.println("You're connected to the MQTT broker!");
      Serial.println();
    #endif
  }


  // Fetch NTP time and date
  init_ntp();
  getNTPtime(10);
  showTime(timeinfo);
  lastNTPtime = time(&now);
  
  unsigned long lastEntryTime;
  lastEntryTime = millis();
  
  getTime();
  dp.updateDateLCD(td.customDay, td.customDate);
  dp.updateTimeLCD(td.customTime);

  // Fetch weather data for cities
  drm = owpDrammen.fetchWeatherData(cityid[0], apikey, &drm);
  ilg = owpIlagan.fetchWeatherData(cityid[1], apikey, &ilg);

  #ifdef DEBUG
    Serial.print(F("Test"));
    Serial.print(F("Location: "));
    Serial.println(drm.getLocation());
    Serial.print(F("Temperature: "));
    Serial.println(drm.getTemperature());
    Serial.print(F("Feels like: "));
    Serial.println(drm.getFeelslike());
    Serial.print(F("Temp min: "));
    Serial.println(drm.getTemp_min());
    Serial.print(F("Temp max: "));
    Serial.println(drm.getTemp_max());
    Serial.print(F("Pressure: "));
    Serial.println(drm.getPressure());
    Serial.print(F("Humidity: "));
    Serial.println(drm.getHumidity());
    Serial.print(F("Weather: "));
    Serial.println(drm.getWeather());
    Serial.print(F("Description: "));
    Serial.println(drm.getDescription());
    Serial.print(F("Wind speed: "));
    Serial.println(drm.getWindspeed());
    Serial.print(F("Wind direction: "));
    Serial.println(drm.getWinddirection());
    Serial.print(F("Wind compass direction: "));
    Serial.println(drm.getWindcompassdirection());
    Serial.print(F("Weather ID: "));
    Serial.println(drm.getWeatherid());
    Serial.println(F(" "));
    Serial.print(F("Location: "));
    Serial.println(ilg.getLocation());
    Serial.print(F("Temperature: "));
    Serial.println(ilg.getTemperature());
    Serial.print(F("Feels like: "));
    Serial.println(ilg.getFeelslike());
    Serial.print(F("Temp min: "));
    Serial.println(ilg.getTemp_min());
    Serial.print(F("Temp max: "));
    Serial.println(ilg.getTemp_max());
    Serial.print(F("Pressure: "));
    Serial.println(ilg.getPressure());
    Serial.print(F("Humidity: "));
    Serial.println(ilg.getHumidity());
    Serial.print(F("Weather: "));
    Serial.println(ilg.getWeather());
    Serial.print(F("Description: "));
    Serial.println(ilg.getDescription());
    Serial.print(F("Wind speed: "));
    Serial.println(ilg.getWindspeed());
    Serial.print(F("Wind direction: "));
    Serial.println(ilg.getWinddirection());
    Serial.print(F("Wind compass direction: "));
    Serial.println(ilg.getWindcompassdirection());
    Serial.print(F("Weather ID: "));
    Serial.println(ilg.getWeatherid());
    Serial.println();
  #endif
}

// Variables to support auto time update
bool timeChanged = true;
bool hourChanged = true;
bool dateChanged = true;
String compareTime;
String compareHour;
String compareDate;

// Variables to support auto page rotation
bool page0Turned = true;
bool page1Turned = false;
bool page2Turned = false;
const unsigned long turnPageInterval = 60UL*1000UL; // How fast the screen pages rotate
unsigned long previousTurnPageTime = 0;

// Variables to support RF connection loss
const unsigned long wait_for_connection = 180000; // 30 sec in ms, after 30sec, conneciton loss alarm!
unsigned long connection_lost;
bool timerSet = false;

/************************************* LOOP ************************************************************/

void loop() {

  // Display icon on LCD if RF connection is lost
  unsigned long now = millis();
  if (now - connection_lost >= wait_for_connection)
  {
    dp.showicon("radioinactive", 1);
  }
  else
  {
    dp.showicon("radioinactive", 0);
  }

  // RF Receive
  uint8_t data[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(data);

  // Checking if RF connection is up and running
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
    dp.showicon("radioactive", 1);

    // Receive data from remote sensor
    if (rf.recv(data, &len)) {
      rfwd* weatherdata = (rfwd*)data;


      // Filling Sensor data class with data from the remote sensor
      sensorData.setTemperature(weatherdata->temperature);
      sensorData.setPressure(weatherdata->pressure);
      sensorData.setHumidity(weatherdata->humidity);
      sensorData.setBattery(weatherdata->battery);
      sensorData.setSensorid(weatherdata->sensorID);

      // Displaying remote sensor data on LCD
      dp.showicon("radioinactive", 0);
      dp.printToLCD("location0.txt", "Verven 30");
      dp.printToLCD("temperature0.txt", String(sensorData.getTemperature()));
      dp.printToLCD("humidity0.txt", String(sensorData.getHumidity()));
      dp.printToLCD("pressure0.txt", String(sensorData.getPressure()));
      dp.printToLCD("battery.txt", String(sensorData.getBattery()));
      dp.showicon("radioactive", 0);


      if (!mqttClient.connect(broker, port)) {
        #ifdef DEBUG
          Serial.print("MQTT connection failed! Error code = ");
          Serial.println(mqttClient.connectError());
        #endif
      } else {
        #ifdef DEBUG
          Serial.println("You're connected to the MQTT broker!");
          Serial.println();
        #endif
      }

      // Sending data to MQTT Broker
      mqttClient.beginMessage(topicTemperature);
      mqttClient.print(sensorData.getTemperature());
      mqttClient.endMessage();

      mqttClient.beginMessage(topicHumidity);
      mqttClient.print(sensorData.getHumidity());
      mqttClient.endMessage();

      mqttClient.beginMessage(topicPressure);
      mqttClient.print(sensorData.getPressure());
      mqttClient.endMessage();
      
      mqttClient.beginMessage(topicBattery);
      mqttClient.print(sensorData.getBattery());
      mqttClient.endMessage();

      #ifdef DEBUG
        Serial.print(F("Temperature: "));
        Serial.print(sensorData.getTemperature());
        Serial.println(F("c"));
        Serial.print(F("Pressure: "));
        Serial.print(sensorData.getPressure());
        Serial.println(F("hpa"));
        Serial.print(F("Humidity: "));
        Serial.print(sensorData.getHumidity());
        Serial.println(F("%"));
        Serial.print(F("Battery: "));
        Serial.print(sensorData.getBattery());
        Serial.println(F("v"));
        Serial.print(F("Sensor ID: "));
        Serial.println(sensorData.getSensorid());
        Serial.println();
      #endif

    }
   }


  // Fetches NTP time and updates the LCD when time and date have changed
  getNTPtime(1000);
  getTime();

  // Auto screen rotation
  unsigned long currentTurnPageTime = millis();
  if (currentTurnPageTime - previousTurnPageTime >= turnPageInterval) {
    
    if(page0Turned){
      dp.turnPage(cityid, "1", td.customTime, td.customDay, td.customDate, ilg, drm, sensorData, WiFi.localIP().toString());
      page0Turned = false;
      page1Turned = true;
    }else if(page1Turned){
      dp.turnPage(cityid, "2", td.customTime, td.customDay, td.customDate, ilg, drm, sensorData, WiFi.localIP().toString());
      page1Turned = false;
      page2Turned = true;
    }else if(page2Turned){
      dp.turnPage(cityid, "0", td.customTime, td.customDay, td.customDate, ilg, drm, sensorData, WiFi.localIP().toString());
      page2Turned = false;
      page0Turned = true;
    }
    previousTurnPageTime += turnPageInterval;
  }

  // Update screen data when screen button is pressed
  dp.readLCDData(cityid, td.customDay, td.customDate, td.customTime, ilg, drm, sensorData, WiFi.localIP().toString());

  // Updating time and date only when time or date has changed

  // Checking if hours has changed
  String oldHour = td.customHour;
  if (hourChanged){
    compareHour = oldHour;
    hourChanged = false;
  }
  
  if (compareHour != oldHour)
  {
    drm = owpDrammen.fetchWeatherData(cityid[0], apikey, &drm);
    ilg = owpIlagan.fetchWeatherData(cityid[1], apikey, &ilg);
    #ifdef DEBUG
      Serial.print(F("Hour: "));
      Serial.println(td.customHour);
    #endif
    
    hourChanged = true;
  }

  // Cheking if time has changed
  String oldTime = td.customTime;
  if (timeChanged)
  {
    compareTime = oldTime;
    timeChanged = false;
  }

  if (compareTime != oldTime)
  {
    dp.updateTimeLCD(td.customTime);
    #ifdef DEBUG
      Serial.print(F("Time: "));
      Serial.println(td.customTime);
    #endif
    
    timeChanged = true;
    mqttClient.poll(); // Polling MQTT Server once every minute.
  }

  // Checking if date has changed
  String oldDate = String(td.customDate);
  if (dateChanged)
  {
    compareDate = oldDate;
    dateChanged = false;
  }

  if (compareDate != oldDate)
  {
    dp.updateDateLCD(td.customDay, td.customDate);
    #ifdef DEBUG
      Serial.print(F("Day: "));
      Serial.println(td.customDay);
      Serial.print(F("Date: "));
      Serial.println(td.customDate);
    #endif
    
    dateChanged = true;
  }
}

// NOT IN USE YET
// TODO: Implement remote sensor "feels like" temperature.
float convertFtoC(float f) { return (f - 32) * 0.55555; }

/*!
 *  @brief  Compute Heat Index
 *          Using both Rothfusz and Steadman's equations
 *          (http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml)
 *  @param  temperature
 *          temperature in selected scale
 *  @param  percentHumidity
 *          humidity in percent
 *  @param  isFahrenheit
 *          true if fahrenheit, false if celcius
 *  @return float heat index
 *  @source https://github.com/adafruit/DHT-sensor-library/blob/master/DHT.cpp
 */
float computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit) {
  float hi;

  hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) +
              (percentHumidity * 0.094));

  if (hi > 79) {
    hi = -42.379 + 2.04901523 * temperature + 10.14333127 * percentHumidity +
         -0.22475541 * temperature * percentHumidity +
         -0.00683783 * pow(temperature, 2) +
         -0.05481717 * pow(percentHumidity, 2) +
         0.00122874 * pow(temperature, 2) * percentHumidity +
         0.00085282 * temperature * pow(percentHumidity, 2) +
         -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);

    if ((percentHumidity < 13) && (temperature >= 80.0) &&
        (temperature <= 112.0))
      hi -= ((13.0 - percentHumidity) * 0.25) *
            sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

    else if ((percentHumidity > 85.0) && (temperature >= 80.0) &&
             (temperature <= 87.0))
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }

  return isFahrenheit ? hi : convertFtoC(hi);
}

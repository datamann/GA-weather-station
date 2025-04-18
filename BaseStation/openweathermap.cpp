/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    07.12.2020
    @see The GNU Public License (GPL) Version 3
*/

#include "openweathermap.h"
#include "WeatherData.h"
#include <ArduinoJson.h>

#ifdef ESP32
  #include <WiFi.h>
#endif

#ifdef ESP8266
  #include <ESP8266WiFi.h>
#endif

// OPENWEATHER API CONFIGURATION
//String cityid =  "3159016";               // -> config.h
//String apikey = "*****************";       // -> credentials.h

OpenWeatherMap::OpenWeatherMap() {}

#ifdef ESP32

WeatherData OpenWeatherMap::fetchWeatherData(String cityid, String apikey, WeatherData *ptr)
{
    String result = "";
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(_servername, httpPort)) {
      #ifdef DEBUG
        Serial.print(F("Client not connected! "));
        Serial.println(client);
      #endif
    }

    // We now create a URI for the request
    String url = "/data/2.5/forecast?id="+ cityid +"&units=metric&cnt=1&APPID="+ apikey;
    
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + _servername + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    
    while (client.available() == 0) {
        if (millis() - timeout > 10000) {
            #ifdef DEBUG
              Serial.print(F("Client timed out! "));
              Serial.println(client);
            #endif
            //client.stop();
            //ESP.restart(); // Restarts ESP to prevent hang.
        }
    }

    // Read all the lines of the reply from server
    while(client.available()) {
        result = client.readStringUntil('\r');
    }

    // Removes angle brackets from API JSON output
    result.replace('[', ' ');
    result.replace(']', ' ');

    char jsonArray [result.length()+1];
    result.toCharArray(jsonArray,sizeof(jsonArray));
    jsonArray[result.length() + 1] = '\0';

    // Memory allocation - Static or dynamic
    //DynamicJsonDocument doc(1024);
    StaticJsonDocument<1024> doc;
    
    DeserializationError error = deserializeJson(doc, jsonArray);
    if (error) {
      #ifdef DEBUG
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
      #endif
    }

    WeatherData *wd  = new WeatherData();

    const char* location = doc["city"]["name"];
    wd->setLocation(location);
    float temperature = doc["list"]["main"]["temp"];
    wd->setTemperature(temperature);
    float feelsLike = doc["list"]["main"]["feels_like"];
    wd->setFeelslike(feelsLike);
    float temp_min = doc["list"]["main"]["temp_min"];
    wd->setTemp_min(temp_min);
    float temp_max = doc["list"]["main"]["temp_max"];
    wd->setTemp_max(temp_max);
    int pressure = doc["list"]["main"]["pressure"];
    wd->setPressure(pressure);
    int humidity = doc["list"]["main"]["humidity"];
    wd->setHumidity(humidity);
    const char* weather = doc["list"]["weather"]["main"];
    wd->setWeather(weather);
    const char* description = doc["list"]["weather"]["description"];
    wd->setDescription(description);
    float windSpeed = doc["list"]["wind"]["speed"];
    wd->setWindspeed(windSpeed);
    int windDirection = doc["list"]["wind"]["deg"];
    wd->setWinddirection(windDirection);
    wd->setWindcompassdirection(getWindDirection(wd->getWinddirection()));
    int weatherID = doc["list"]["weather"]["id"];
    wd->setWeatherid(weatherID);

  // TODO: Remove
    /*Serial.print(F("Location: "));
    Serial.println(wd->getLocation());
    Serial.print(F("Temperature: "));
    Serial.println(wd->getTemperature());
    Serial.print(F("Feels like: "));
    Serial.println(wd->getFeelslike());
    Serial.print(F("Temp min: "));
    Serial.println(wd->getTemp_min());
    Serial.print(F("Temp max: "));
    Serial.println(wd->getTemp_max());
    Serial.print(F("Pressure: "));
    Serial.println(wd->getPressure());
    Serial.print(F("Humidity: "));
    Serial.println(wd->getHumidity());
    Serial.print(F("Weather: "));
    Serial.println(wd->getWeather());
    Serial.print(F("Description: "));
    Serial.println(wd->getDescription());
    Serial.print(F("Wind speed: "));
    Serial.println(wd->getWindspeed());
    Serial.print(F("Wind direction: "));
    Serial.println(wd->getWinddirection());
    Serial.print(F("Wind compass direction: "));
    Serial.println(wd->getWindcompassdirection());
    Serial.print(F("Weather ID: "));
    Serial.println(wd->getWeatherid());*/

/*{"cod":"200","message":0,"cnt":1,

"list":{
  "dt":1589500800,
      "main"{
        "temp":2.71,
        "feels_like":-0.19,
        "temp_min":1.58,
        "temp_max":2.71,
        "pressure":1009,
        "sea_level":1010,
        "grnd_level":1010,
        "humidity":73,
        "temp_kf":1.13
       },
      "weather":{
        "id":800,
        "main":"Clear",
        "description":"clear sky",
        "icon":"01n"
       },
      "clouds":{
        "all":8
       },
      "wind":{
        "speed":0.98,
        "deg":282
       },
      "sys":{
        "pod":"n"
       },
  "dt_txt":"2020-05-15 00:00:00"
  },
  "city":{
    "id":3159016,
    "name":"Drammen",
  "coord":{
    "lat":59.7439,
    "lon":10.2045
    },
  "country":"NO",
  "timezone":7200,
  "sunrise":1589424285,
  "sunset":1589485588
  }
}*/
  return *wd;
};
#endif


#ifdef ESP8266

WeatherData OpenWeatherMap::fetchWeatherData (String cityid, String apikey, WeatherData *ptr) {

  String result;

  WiFiClient httpclient = wifiConnect(cityid, apikey);
  unsigned long timeout = millis();

  while (httpclient.connected() == 0) {

    if (millis() - timeout > 10000) {
      httpclient.stop();

      #ifdef DEBUG
        Serial.print(F("HTTP client timed out! "));
        Serial.println(httpclient);
      #endif
      
      httpclient = wifiConnect(cityid, apikey);

      #ifdef DEBUG
        Serial.print(F("Tries to reconnect! "));
        Serial.println(httpclient.connected());
      #endif
    }
  }
  
  while (httpclient.connected() || httpclient.available()) {
    char c = httpclient.read();

    if (c != 255){
      result = result+c;
    }
  }
  httpclient.stop();

  result.replace('[', ' ');
  result.replace(']', ' ');

  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonArray);

  if (error) {
    #ifdef DEBUG
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(error.c_str());
    #endif
  }

  WeatherData *wd  = new WeatherData();

  const char* location = doc["name"];
  wd->setLocation(location);
  float temperature = doc["main"]["temp"];
  wd->setTemperature(temperature);
  float feelsLike = doc["main"]["feels_like"];
  wd->setFeelslike(feelsLike);
  float temp_min = doc["main"]["temp_min"];
  wd->setTemp_min(temp_min);
  float temp_max = doc["main"]["temp_max"];
  wd->setTemp_max(temp_max);
  int pressure = doc["main"]["pressure"];
  wd->setPressure(pressure);
  int humidity = doc["main"]["humidity"];
  wd->setHumidity(humidity);
  const char* weather = doc["weather"]["main"];
  wd->setWeather(weather);
  const char* description = doc["weather"]["description"];
  wd->setDescription(description);
  float windSpeed = doc["wind"]["speed"];
  wd->setWindspeed(windSpeed);
  int windDirection = doc["wind"]["deg"];
  wd->setWinddirection(windDirection);
  wd->setWindcompassdirection(getWindDirection(wd->getWinddirection()));
  int weatherID = doc["weather"]["id"];
  wd->setWeatherid(weatherID);

  // TODO: Remove
    /*Serial.print(F("Location: "));
    Serial.println(wd->getLocation());
    Serial.print(F("Temperature: "));
    Serial.println(wd->getTemperature());
    Serial.print(F("Feels like: "));
    Serial.println(wd->getFeelslike());
    Serial.print(F("Temp min: "));
    Serial.println(wd->getTemp_min());
    Serial.print(F("Temp max: "));
    Serial.println(wd->getTemp_max());
    Serial.print(F("Pressure: "));
    Serial.println(wd->getPressure());
    Serial.print(F("Humidity: "));
    Serial.println(wd->getHumidity());
    Serial.print(F("Weather: "));
    Serial.println(wd->getWeather());
    Serial.print(F("Description: "));
    Serial.println(wd->getDescription());
    Serial.print(F("Wind speed: "));
    Serial.println(wd->getWindspeed());
    Serial.print(F("Wind direction: "));
    Serial.println(wd->getWinddirection());
    Serial.print(F("Wind compass direction: "));
    Serial.println(wd->getWindcompassdirection());
    Serial.print(F("Weather ID: "));
    Serial.println(wd->getWeatherid());*/

  return *wd;
};

WiFiClient OpenWeatherMap::wifiConnect(String cityid, String apikey){
  WiFiClient httpclient;

  // IP: 37.139.20.5
  // Host: api.openweathermap.org

  // WiFiClient doesn't do DNS resolving very good so IP address is used.
  char servername[]="37.139.20.5";
  httpclient.connect(servername, 80);

  httpclient.println("GET /data/2.5/weather?id="+cityid+"&units=metric&APPID="+apikey);
  httpclient.println("Host: api.openweathermap.org");
  httpclient.println("User-Agent: ArduinoWiFi/1.1");
  httpclient.println("Connection: close");
  httpclient.println();

  return httpclient;
}
#endif

const char* OpenWeatherMap::getWindDirection(int deg)
{
  float wind_deg = float(deg);
  const char* direction = "---";

  if(OpenWeatherMap::inRange(wind_deg, 348.750, 360.999)){direction = "N";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 000.000, 11.249)){direction = "N";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 11.250, 33.749)){direction = "NNE";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 33.750, 56.249)){direction = "NE";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 56.250, 78.749)){direction = "ENE";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 78.750, 101.249)){direction = "E";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 101.250, 123.749)){direction = "ESE";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 123.750, 146.249)){direction = "SE";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 146.250, 168.749)){direction = "SSE";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 168.750, 191.249)){direction = "S";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 191.250, 213.749)){direction = "SSW";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 213.750, 236.249)){direction = "SW";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 236.250, 258.749)){direction = "WSW";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 258.750, 281.249)){direction = "W";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 281.250, 303.749)){direction = "WNW";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 303.750, 326.249)){direction = "NW";return direction;}
  if(OpenWeatherMap::inRange(wind_deg, 326.250, 348.749)){direction = "NNW";return direction;}
};

bool OpenWeatherMap::inRange(float val, float minimum, float maximum)
{
  return ((minimum <= val) && (val <= maximum));
};

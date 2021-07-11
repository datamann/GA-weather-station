/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    01.12.2020
    @see The GNU Public License (GPL) Version 3
*/
#ifdef ESP32
  #ifndef WIFI_h
    #define WIFI_h
    #include <WiFi.h>
  #endif
#endif

#ifdef ESP8266
  #ifndef ESP8266WiFi_h
    #define ESP8266WiFi_h
    #include <ESP8266WiFi.h>
  #endif
#endif

// WIFI
//char ssid[]           = "YOUR_SSID";    // -> credentials.h
//const char* password  = "***********";  // -> credentials.h

WiFiClient init_Wifi() {

#ifdef ESP32
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  WiFiClient client;
#endif
#ifdef ESP8266
  WiFi.mode(WIFI_STA);
  WiFi.softAPdisconnect(true);
  WiFi.begin(ssid, password);
  WiFiClient client;
#endif
 return client;
}

int check_Wifi_Connected() {

  int status = WiFi.status();
  switch (status) {
    case WL_NO_SHIELD: return WL_NO_SHIELD;
    case WL_IDLE_STATUS: return WL_IDLE_STATUS;
    case WL_NO_SSID_AVAIL: return WL_NO_SSID_AVAIL;
    case WL_SCAN_COMPLETED: return WL_SCAN_COMPLETED;
    case WL_CONNECTED: return WL_CONNECTED;
    case WL_CONNECT_FAILED: return WL_CONNECT_FAILED;
    case WL_CONNECTION_LOST: return WL_CONNECTION_LOST;
    case WL_DISCONNECTED: return WL_DISCONNECTED;
  }
}

// TODO: Remove
/*
 // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WEP network, SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, keyIndex, key);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // once you are connected :
  Serial.print("You're connected to the network");
  */
//#endif

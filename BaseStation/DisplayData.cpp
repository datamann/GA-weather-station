/*
    Written by Stig B. Sivertsen
    sbsivertsen@gmail.com
    https://github.com/datamann/GA-weather-station
    24.09.2020
    @see The GNU Public License (GPL) Version 3
*/

#include "DisplayData.h"
#include "lcd.h"
#include "WeatherData.h"
#include "SensorData.h"

/*
 * String CityID1 = "1711146"; // Ilagan
 * String CityID2 = "3159016"; // Drammen
 * String cityid[3] = {"3159016","1711146"}; // Drammen, Ilagan
*/

String RemoteSensorLocation = "Skogliveien 14";

DisplayData::DisplayData() {
  init_LCD();
};

void DisplayData::readLCDData(String* cityid, String Day, String Date, String Time, WeatherData City1, WeatherData City2, SensorData sensorData, String IP){

/* Packet sent from Nextion touch screen when onscreen button is pressed:
 * Start:   101
 * Page:    0/1/2
 * Component Id:  32/30/30
 * Event type (1=pressed/0=release): 1/1/1
 * End of message: 255 255 255
*/
  
  int actionid = 0;
  int pageid = 1000;
  int componentid = 1000;

  // Reading data from touch screen
  while (lcd.available())
  {
      actionid = lcd.read();
      
      if (actionid == 101){
        pageid = lcd.read();
        componentid = lcd.read();
      }
  }

  if (pageid == 0) {
    DisplayData::showicon("radioactive", 0);
    DisplayData::printWeatherData(cityid[0], City1, IP);
    pageid = 100;
  }
  if (pageid == 1) {
    DisplayData::showicon("radioactive", 0);
    DisplayData::printWeatherData(cityid[1], City2, IP);
    pageid = 100;
  }
  if (pageid == 2){
    DisplayData::showicon("radioactive", 0);
    DisplayData::updateDateLCD(Day, Date);
    DisplayData::updateTimeLCD(Time);
    DisplayData::printToLCD("location0.txt", RemoteSensorLocation);
    DisplayData::printToLCD("temperature0.txt", String(sensorData.getTemperature()));
    DisplayData::printToLCD("humidity0.txt", String(sensorData.getHumidity()));
    DisplayData::printToLCD("pressure0.txt", String(sensorData.getPressure()));
    DisplayData::printToLCD("battery.txt", String(sensorData.getBattery()));
    DisplayData::getWeatherIcon(int(City2.getWeatherid()),"1");
    DisplayData::printToLCD("ipaddress.txt", IP);
    pageid = 100;
  }
};

void DisplayData::updateDateLCD(String Day, String Date)
{
    // Day
    lcd.print("day.txt=");
    lcd.print("\"");
    lcd.print(Day);
    lcd.print("\"");
    DisplayData::endNextionCommand();
  
    // Date
    lcd.print("date.txt=");
    lcd.print("\"");
    lcd.print(Date);
    lcd.print("\"");
    DisplayData::endNextionCommand();
};

void DisplayData::updateTimeLCD(String Time)
{
    // Hours and minutes
    lcd.print("time.txt=");
    lcd.print("\"");
    lcd.print(Time);
    lcd.print("\"");
    DisplayData::endNextionCommand();
};

void DisplayData::printToLCD(String field, String data)
{
  lcd.print(""+ field +"=");
  lcd.print("\"");
  lcd.print(data);
  lcd.print("\"");
  DisplayData::endNextionCommand();
};

void DisplayData::showicon(String icon, int action)
{
  lcd.print("vis "+ icon +","+ action +"");
  DisplayData::endNextionCommand();
};

void DisplayData::showConnectingIcon(String pageid, String imageid)
{
  String command = "weatherIcon"+ pageid +".pic="+ imageid +"";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::turnPage(String* cityid, String pageid, char* Time, char* Day, char* Date, WeatherData City1, WeatherData City2, SensorData sensorData, String IP)
{
  String command = "page "+ pageid + "";
  lcd.print(command);
  DisplayData::endNextionCommand();

  if(pageid == "0"){
    DisplayData::showicon("radioactive", 0);
    DisplayData::updateDateLCD(Day, Date);
    DisplayData::updateTimeLCD(Time);
    DisplayData::printToLCD("location0.txt", RemoteSensorLocation);
    DisplayData::printToLCD("ipaddress.txt", IP);
    DisplayData::printToLCD("temperature0.txt", String(sensorData.getTemperature()));
    DisplayData::printToLCD("humidity0.txt", String(sensorData.getHumidity()));
    DisplayData::printToLCD("pressure0.txt", String(sensorData.getPressure()));
    DisplayData::printToLCD("battery.txt", String(sensorData.getBattery()));
    DisplayData::getWeatherIcon(int(City1.getWeatherid()),"1");
    DisplayData::printToLCD("ipaddress.txt", IP);
  }
  if(pageid == "1"){
    DisplayData::showicon("radioactive", 0);
    DisplayData::printWeatherData(cityid[0], City1, IP);
  }
  if(pageid == "2"){
    DisplayData::showicon("radioactive", 0);
    DisplayData::printWeatherData(cityid[1], City2, IP);
  }
};

void DisplayData::endNextionCommand()
{
  lcd.write(0xff);
  lcd.write(0xff);
  lcd.write(0xff);
};

void DisplayData::drawFog(String pageid)
{
  String command = "weathericon"+ pageid +".pic=11";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawHeavySnowfall(String pageid)
{
  String command = "weathericon"+ pageid +".pic=6";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawModerateSnowfall(String pageid)
{
  String command = "weathericon"+ pageid +".pic=6";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawLightSnowfall(String pageid)
{
  String command = "weathericon"+ pageid +".pic=11";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawHeavyRain(String pageid)
{
  String command = "weathericon"+ pageid +".pic=8";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawModerateRain(String pageid)
{
  String command = "weathericon"+ pageid +".pic=8";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawLightRain(String pageid)
{
  String command = "weathericon"+ pageid +".pic=4";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawLightRainWithSunOrMoon(String pageid)
{
  String command = "weathericon"+ pageid +".pic=5";
  lcd.print(command);
  DisplayData::endNextionCommand(); 
};

void DisplayData::drawThunderstorm(String pageid)
{
  String command = "weathericon"+ pageid +".pic=1";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawClearWeather(String pageid)
{
  String command = "weathericon"+ pageid +".pic=2";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawCloud(String pageid)
{
  String command = "weathericon"+ pageid +".pic=7";
  lcd.print(command);
  DisplayData::endNextionCommand();
};

void DisplayData::drawFewClouds(String pageid)
{
  String command = "weathericon"+ pageid +".pic=3";
  lcd.print(command);
  DisplayData::endNextionCommand(); 
};

const char* DisplayData::getWeatherIcon(int id, String pageid)
{
 switch(id)
 {
  case 800: DisplayData::drawClearWeather(pageid); break;
  case 801: DisplayData::drawFewClouds(pageid); break;
  case 802: DisplayData::drawFewClouds(pageid); break;
  case 803: DisplayData::drawCloud(pageid); break;
  case 804: DisplayData::drawCloud(pageid); break;
  
  case 200: DisplayData::drawThunderstorm(pageid); break;
  case 201: DisplayData::drawThunderstorm(pageid); break;
  case 202: DisplayData::drawThunderstorm(pageid); break;
  case 210: DisplayData::drawThunderstorm(pageid); break;
  case 211: DisplayData::drawThunderstorm(pageid); break;
  case 212: DisplayData::drawThunderstorm(pageid); break;
  case 221: DisplayData::drawThunderstorm(pageid); break;
  case 230: DisplayData::drawThunderstorm(pageid); break;
  case 231: DisplayData::drawThunderstorm(pageid); break;
  case 232: DisplayData::drawThunderstorm(pageid); break;

  case 300: DisplayData::drawLightRain(pageid); break;
  case 301: DisplayData::drawLightRain(pageid); break;
  case 302: DisplayData::drawLightRain(pageid); break;
  case 310: DisplayData::drawLightRain(pageid); break;
  case 311: DisplayData::drawLightRain(pageid); break;
  case 312: DisplayData::drawLightRain(pageid); break;
  case 313: DisplayData::drawLightRain(pageid); break;
  case 314: DisplayData::drawLightRain(pageid); break;
  case 321: DisplayData::drawLightRain(pageid); break;

  case 500: DisplayData::drawLightRainWithSunOrMoon(pageid); break;
  case 501: DisplayData::drawLightRainWithSunOrMoon(pageid); break;
  case 502: DisplayData::drawLightRainWithSunOrMoon(pageid); break;
  case 503: DisplayData::drawLightRainWithSunOrMoon(pageid); break;
  case 504: DisplayData::drawLightRainWithSunOrMoon(pageid); break;
  case 511: DisplayData::drawLightRain(pageid); break;
  case 520: DisplayData::drawModerateRain(pageid); break;
  case 521: DisplayData::drawModerateRain(pageid); break;
  case 522: DisplayData::drawHeavyRain(pageid); break;
  case 531: DisplayData::drawHeavyRain(pageid); break;

  case 600: DisplayData::drawLightSnowfall(pageid); break;
  case 601: DisplayData::drawModerateSnowfall(pageid); break;
  case 602: DisplayData::drawHeavySnowfall(pageid); break;
  case 611: DisplayData::drawLightSnowfall(pageid); break;
  case 612: DisplayData::drawLightSnowfall(pageid); break;
  case 615: DisplayData::drawLightSnowfall(pageid); break;
  case 616: DisplayData::drawLightSnowfall(pageid); break;
  case 620: DisplayData::drawLightSnowfall(pageid); break;
  case 621: DisplayData::drawModerateSnowfall(pageid); break;
  case 622: DisplayData::drawHeavySnowfall(pageid); break;

  case 701: DisplayData::drawFog(pageid); break;
  case 711: DisplayData::drawFog(pageid); break;
  case 721: DisplayData::drawFog(pageid); break;
  case 731: DisplayData::drawFog(pageid); break;
  case 741: DisplayData::drawFog(pageid); break;
  case 751: DisplayData::drawFog(pageid); break;
  case 761: DisplayData::drawFog(pageid); break;
  case 762: DisplayData::drawFog(pageid); break;
  case 771: DisplayData::drawFog(pageid); break;
  case 781: DisplayData::drawFog(pageid); break;

  default:break; 
 }
};

void DisplayData::printWeatherData(String cityid, WeatherData City, String IP)
{

  if (cityid == "3159016") // Drammen, NO
  {
    printToLCD("temperature1.txt", String(City.getTemperature()));
    printToLCD("tempfeelslike1.txt", String(City.getFeelslike()));
    printToLCD("tempmin1.txt", String(City.getTemp_min()));
    printToLCD("tempmax1.txt", String(City.getTemp_max()));
    printToLCD("pressure1.txt", String(City.getPressure()));
    printToLCD("humidity1.txt", String(City.getHumidity()));
    printToLCD("description1.txt", String(City.getDescription()));
    printToLCD("windspeed1.txt", String(City.getWindspeed()));
    printToLCD("winddirection1.txt", String(City.getWindcompassdirection()));
    printToLCD("windDirection1.txt", String(City.getWinddirection()));
    printToLCD("location1.txt", String(City.getLocation()));
    printToLCD("ipaddress.txt", IP);
    getWeatherIcon(int(City.getWeatherid()),"1");
  }
  else if(cityid == "1711146") // Ilagan, PH
  {
    printToLCD("temperature2.txt", String(City.getTemperature()));
    printToLCD("tempfeelslike2.txt", String(City.getFeelslike()));
    printToLCD("tempmin2.txt", String(City.getTemp_min()));
    printToLCD("tempmax2.txt", String(City.getTemp_max()));
    printToLCD("pressure2.txt", String(City.getPressure()));
    printToLCD("humidity2.txt", String(City.getHumidity()));
    printToLCD("description2.txt", String(City.getDescription()));
    printToLCD("windspeed2.txt", String(City.getWindspeed()));
    printToLCD("winddirection2.txt", String(City.getWindcompassdirection()));
    printToLCD("windDirection2.txt", String(City.getWinddirection()));
    printToLCD("location2.txt", String(City.getLocation()));
    printToLCD("ipaddress.txt", IP);
    getWeatherIcon(int(City.getWeatherid()),"2");
  }
};

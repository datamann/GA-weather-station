#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;

#define DEBUG

int FanSwitch = 13;   // Pin D13
int PumpSwitch = 12;  // Pin D12
int SoilSensor = 0;   // Pin A0
int WaterSensor = 1;  // Pin A1
int VoltageProbe = 2; // Pin A2
float volt;

struct wd {
  float temperature;
  float humidity;
  float battery;
  uint8_t sensorID = 2;
}; wd weatherdata;

void setup() {
  Serial.begin(9600);
  pinMode(FanSwitch,OUTPUT);
  pinMode(PumpSwitch,OUTPUT);
  pinMode(SoilSensor,OUTPUT);
  pinMode(WaterSensor,OUTPUT);
  pinMode(VoltageProbe,OUTPUT);

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
}

void readWeatherData(){
    weatherdata.temperature = 0;
    weatherdata.humidity = 0;
    
    weatherdata.temperature = bme.readTemperature();
    weatherdata.humidity = bme.readHumidity();
}

float voltsIn;
void readVoltage(){
  weatherdata.battery = 0;
  volt = 0.00;
  voltsIn = (unsigned) analogRead(VoltageProbe);
  volt = voltsIn * 0.013489736070381;   // 13.8v/1023 = 0,013489736070381
  weatherdata.battery = volt;
}

void Fan(bool onOff){
  if (onOff){
    digitalWrite(FanSwitch, HIGH);
  }
  else{
    digitalWrite(FanSwitch, LOW);
  }
}

void Pump(bool onOff){
  if (onOff){
    digitalWrite(PumpSwitch, HIGH);
  }
  else{
    digitalWrite(PumpSwitch, LOW);
  }
}

void loop() {
  readWeatherData();
  readVoltage();
  printValues();

  if (weatherdata.temperature > 29){
    Serial.println("Temerature is greater!");
    Fan(1);
    Pump(0);
  }
  else{
    Serial.println("Temerature is not greater!");
    Fan(0);
    Pump(1);
  }
  delay(1000);
}

void printValues() {

    #ifdef DEBUG
        char buf[16];
      
        char temperature[6];
        char humidity[3];
        char batt[5];
        char id[2];
      
        dtostrf(weatherdata.temperature, 3, 2, temperature);
        dtostrf(weatherdata.humidity, 2, 0, humidity);
        dtostrf(weatherdata.battery, 2, 2, batt);
        dtostrf(weatherdata.sensorID, 1, 0, id);

        sprintf(buf,"%s %s %s %s", temperature, humidity, batt, id);
        Serial.print(F("Buf: "));
        Serial.println(buf);
    #endif
}

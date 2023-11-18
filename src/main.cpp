#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include <HomeSpan.h>
#include "GyverButton.h"

#include <NTPClient.h>
#include <WiFiUdp.h>

#include "helpers.h"
#include "secrets.h"
//  pair code 46637726

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define MAX_DEVICES 4
#define CS_PIN 5

#define MODE_BUTTON_PIN 13

#define MODE_TIME 1
#define MODE_TEMPERATURE 2
#define MODE_HUMIDITY 3




Adafruit_BME280 bme;
unsigned long delayTime = 5000;
void printValues();

bool isBmeConnected;

int lastMillis = 0;
bool showTemp = true;

MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

WiFiUDP ntpUDP;

NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600 * 2, 60000);

GButton modeButton(MODE_BUTTON_PIN);

struct DEV_TempSensor : Service::TemperatureSensor {

  SpanCharacteristic *temp;                         // reference to the Current Temperature Characteristic
  
  DEV_TempSensor() : Service::TemperatureSensor(){       // constructor() method
    
    temp=new Characteristic::CurrentTemperature(-10.0);        // instantiate the Current Temperature Characteristic
    temp->setRange(-50,100);                                  // expand the range from the HAP default of 0-100 to -50 to 100 to allow for negative temperatures
        
    Serial.print("Configuring Temperature Sensor");           // initialization message
    Serial.print("\n");

  }

  void loop(){
    temp->setVal(bme.readTemperature());
  }
};

struct DEV_HumSensor : Service::HumiditySensor {

  SpanCharacteristic *humidity;                         // reference to the Current Temperature Characteristic
  
  DEV_HumSensor() : Service::HumiditySensor(){       // constructor() method
    
    humidity=new Characteristic::CurrentRelativeHumidity(0);        // instantiate the Current Temperature Characteristic
    // humidity->setRange(0,100);                                  // expand the range from the HAP default of 0-100 to -50 to 100 to allow for negative temperatures
        
    Serial.print("Configuring Humidity Sensor");           // initialization message
    Serial.print("\n");

  }

  void loop(){
    humidity->setVal(bme.readHumidity());
  }
};

int mode = MODE_TIME;

void setup() {
  Serial.begin(115200);

  Serial.println("Setup start");

  Display.begin();
  Display.setIntensity(0);
  Display.displayClear();

  isBmeConnected = bme.begin(0x76);

  homeSpan.setLogLevel(1);
  homeSpan.setWifiCredentials(wifi_ssid, wifi_pwd);

  homeSpan.begin(Category::Sensors,"Awesome meteo station");
  // new SpanAccessory();  
  //     new Service::AccessoryInformation();
  //       new Characteristic::Identify(); 
        
    new SpanAccessory();
      new Service::AccessoryInformation();
        new Characteristic::Identify(); 
        new Characteristic::Name("Awesome temperature Sensor");
      new DEV_TempSensor();                                                                // Create a Temperature Sensor (see DEV_Sensors.h for definition)

    new SpanAccessory();
      new Service::AccessoryInformation();
        new Characteristic::Identify(); 
        new Characteristic::Name("Awesome humidity sensor");  
      new DEV_HumSensor();    


  WiFi.begin(wifi_ssid, wifi_pwd);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
  timeClient.forceUpdate();


  Serial.println("Setup End");
}

void loop() { 
  homeSpan.poll();
  modeButton.tick();

  if (modeButton.isClick()) {
    if (mode == MODE_TIME) {
      mode = MODE_TEMPERATURE;
    } else if (mode == MODE_TEMPERATURE) {
      mode = MODE_HUMIDITY;
    } else {
      mode = MODE_TIME;
    }
  }

  Display.setTextAlignment(PA_CENTER);


  if ((millis() - lastMillis) > 200) {
    lastMillis = millis();

    if (mode == MODE_TIME) {
        Display.print(getFormattedTime(timeClient));
    } else if (mode == MODE_TEMPERATURE) {
      if (isBmeConnected) {
          Display.printf("%.1f C", bme.readTemperature());
      } else {
        Serial.println("Temperature sensor no connect");
        Display.print("C:err");
      } 
    } else {
      if (isBmeConnected) {
        Display.printf("H: %d ", (int) bme.readHumidity());
      } else {
        Serial.println("Humidity sensor no connect");
        Display.print("H:err");
      }
    }
  }  
}

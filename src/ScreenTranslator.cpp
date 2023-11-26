#include <Arduino.h>
#include "ScreenTranslator.h"

ScreenTranslator::ScreenTranslator(int doRealTickEveryMillisCnfig)
{
  doRealTickEveryMillis = doRealTickEveryMillisCnfig;

  mode = MODE_TIME;
}

int ScreenTranslator::getMode()
{
  return mode;
}

void ScreenTranslator::onButtonClick(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient)
{
  Serial.println("on button click");
  changeMode();
  realTick(bme, display, timeClient);
}

void ScreenTranslator::changeMode()
{
  if (mode == MODE_TIME) {
    mode = MODE_TEMPERATURE;
  } else if (mode == MODE_TEMPERATURE) {
    mode = MODE_HUMIDITY;
  } else if (mode == MODE_HUMIDITY) {
    mode = MODE_OFF;
  } else {
    mode = MODE_TIME;
  }
}

void ScreenTranslator::realTick(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient)
{
  Serial.println("real tick");
  Serial.println(millis());

  updateDisplayState(bme, display, timeClient);
}

void ScreenTranslator::updateDisplayState(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient)
{
  if (mode == MODE_TIME) {
      display.print(getFormattedTime(timeClient));
    } else if (mode == MODE_TEMPERATURE) {
      display.printf("%.1f C", bme.readTemperature());
    } else if (mode == MODE_HUMIDITY) {
      display.printf("H: %d ", (int) bme.readHumidity());
    } else if (mode == MODE_OFF) {
      display.displayClear();
    } else {
      display.print("mErr");
    }
}

void ScreenTranslator::tick(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient)
{
  int now = millis();
  if ((now - lastRealTickMillis) > doRealTickEveryMillis) {
    realTick(bme, display, timeClient);
    lastRealTickMillis = now;
  }
}

String ScreenTranslator::getFormattedTime(NTPClient& timeClient)
{
    unsigned long rawTime = timeClient.getEpochTime();
  unsigned long hours = (rawTime % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  unsigned long seconds = rawTime % 60;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return hoursStr + ":" + minuteStr;
}

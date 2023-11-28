#include <MD_Parola.h>
#include <Adafruit_BME280.h>
#include <NTPClient.h>
#include <Arduino.h>

#define MODE_TIME 1
#define MODE_TEMPERATURE 2
#define MODE_HUMIDITY 3
#define MODE_OFF 4

class ScreenTranslator
{
  public:
    ScreenTranslator(Adafruit_BME280& _bme, MD_Parola& _display, NTPClient& _timeClient): bme(_bme),display(_display), timeClient(_timeClient) {
      mode = MODE_TIME;
      lastRareTickMillis = 0;
      lastButtonClickMillis = 0;
    };
    void tick()
    {
      int now = millis();
      if ((now - lastRealTickMillis) > doRealTickEveryMillis) {
        realTick();
        lastRealTickMillis = now;
      }
    }
    void onButtonClick()
    {
      Serial.println("on button click");
      lastButtonClickMillis = millis();
      changeMode();
      realTick();
    }

  private:
    int mode;
    int lastRealTickMillis;
    int lastRareTickMillis;
    int lastButtonClickMillis;
    int doRealTickEveryMillis = 500;

    Adafruit_BME280& bme;
    MD_Parola& display;
    NTPClient& timeClient;
    
    void realTick()
    {
      Serial.println("real tick");
      Serial.println(millis());

      updateDisplayState();

      int now = millis();
      if ((now - lastRareTickMillis) > 3000) {
        rareTick();
        lastRareTickMillis = now;
      }
    }

    void changeMode()
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
    void updateDisplayState()
    {
      if (mode == MODE_TIME) {
          display.print(getFormattedTime());
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

    void rareTick()
    {
      Serial.println("rare tick");

      if ((millis() - lastButtonClickMillis) < 5000) {
          Serial.println("click was moment ago, skip logic");
          return;
      }

      bool sleepTime = isNightTime();


      if (mode != MODE_OFF && sleepTime) {
        mode = MODE_OFF;
        Serial.println("force off display");
      }

      if (mode == MODE_OFF && !sleepTime) {
        mode = MODE_TIME;
        Serial.println("force on display");
      }
    }

    bool isNightTime()
    {
      unsigned long rawTime = timeClient.getEpochTime();
      unsigned long hours = (rawTime % 86400L) / 3600;
      unsigned long minutes = (rawTime % 3600) / 60;

      return (
        hours >= 22
      ) || (
         hours <= 7
      )
      ;
    }

    String getFormattedTime()
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
};

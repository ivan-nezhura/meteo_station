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
  private:
    int mode;
    int lastRealTickMillis;
    int doRealTickEveryMillis;
    
    void realTick(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient);
    void changeMode();
    void updateDisplayState(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient);
    String getFormattedTime(NTPClient& client);

  public:
    ScreenTranslator(int doRealTickEveryMillis);
    void tick(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient);
    void onButtonClick(Adafruit_BME280& bme, MD_Parola& display, NTPClient& timeClient);
    int getMode();
};

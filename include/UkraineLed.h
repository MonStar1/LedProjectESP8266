#include "BaseLed.h"
#include "LedConfig.h"
#include "LogDebug.h"

String blinker = "mmamammmmammamamaaamammmaa";
int counter = 0;

class UkraineLed : public BaseLed
{
private:
    bool isInitialized = false;

public:
    void loop()
    {
        EVERY_N_MILLIS(100)
        {
            fill_solid(leds, NUM_LEDS, DirectSunlight);
            counter++;
            int light = 255;

            if (counter >= blinker.length())
            {
                counter = 0;
            }

            if (blinker.charAt(counter) == 'a')
            {
                light = 0;
            }

            FastLED.setBrightness(light);
            FastLED.show();
        }
    }
};
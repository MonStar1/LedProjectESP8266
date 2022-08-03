#include "BaseLed.h"
#include "LedConfig.h"

class DaggerLed : public BaseLed
{
private:
    int counter = 0;
    int position = NUM_LEDS;
    CHSV color = CHSV(random8(), 255, 255);

public:
    void loop()
    {
        fadeToBlackBy(leds, NUM_LEDS, 2);
        EVERY_N_MILLISECONDS(200)
        {
            leds[counter++] = color;
            if (counter == NUM_LEDS)
            {
                counter = 0;
                color = CHSV(random8(), 255, 255);
            }
        }
        FastLED.show();
    }
};
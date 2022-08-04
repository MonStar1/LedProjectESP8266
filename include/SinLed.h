#include "BaseLed.h"
#include "LedConfig.h"

class SinLed : public BaseLed
{
private:
    CHSV color = CHSV(random8(), 255, 255);

public:
    void loop()
    {
        fadeToBlackBy(leds, NUM_LEDS, 2);

        int pos = beatsin8(10, 0, NUM_LEDS, 0, 0);
        leds[pos] = color;

        if (pos == 0 || pos == NUM_LEDS)
        {
            color = CHSV(random8(), 255, 255);
        }
        
        FastLED.show();
    }
};
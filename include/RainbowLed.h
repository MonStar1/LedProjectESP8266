#include "BaseLed.h"
#include "LedConfig.h"

class RainbowLed : public BaseLed
{
private:
    uint8_t initialHue = 0;
    CRGB leds2[NUM_LEDS];
    CRGB output[NUM_LEDS];
    long timebase = 0;

public:
    void loop()
    {
        blend(leds, leds2, output, NUM_LEDS, beat8(60 * 5, timebase));
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = output[i];
        }
        EVERY_N_MILLISECONDS(200)
        {
            timebase = millis();
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = leds2[i];
            }
            fill_rainbow(leds2, NUM_LEDS, ++initialHue, 255 / NUM_LEDS);
        }
        FastLED.show();
    }
};
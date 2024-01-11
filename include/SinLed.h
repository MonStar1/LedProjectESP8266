#include "BaseLed.h"
#include "LedConfig.h"

class SinLed : public BaseLed
{
private:
    bool mode = true;

public:
    void loop()
    {
        EVERY_N_MILLIS(350)
        {
            mode = !mode;

            if (mode)
            {
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    if (i % 2 == 0)
                    {
                        leds[i] = CHSV(18, 255, 255);
                    }
                    else
                    {
                        leds[i] = CHSV(21, 222, 255);
                    }
                }
            }
            else
            {
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    if (i % 2 == 0)
                    {
                        leds[i] = CHSV(21, 222, 255);
                    }
                    else
                    {
                        leds[i] = CHSV(18, 255, 255);
                    }
                }
            }
        }

        FastLED.show();
    }
};
#include "BaseLed.h"
#include "LedConfig.h"
#include "LogDebug.h"

class LoadingLed : public BaseLed
{
private:
    bool mode = true;

public:
    void loop()
    {
        EVERY_N_MILLIS(500)
        {
            mode = !mode;

            if (mode)
            {
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    if (i % 2 == 0)
                    {
                        leds[i] = CHSV(255, 255, 255);
                    }
                    else
                    {
                        leds[i] = CHSV(255, 255, 150);
                    }
                }
            }
            else
            {
                for (int i = 0; i < NUM_LEDS; i++)
                {
                    if (i % 2 == 0)
                    {
                        leds[i] = CHSV(255, 255, 150);
                    }
                    else
                    {
                        leds[i] = CHSV(255, 255, 255);
                    }
                }
            }
        }

        FastLED.show();
    }
};
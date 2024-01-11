#include "BaseLed.h"
#include "LedConfig.h"

class BlackLed : public BaseLed
{
private:
    bool executed = false;
    bool firstTime = true;

public:
    void loop()
    {
        if (!executed)
        {
            fadeToBlackBy(leds, NUM_LEDS, 1);
            FastLED.show();

            EVERY_N_SECONDS(2)
            {
                if (firstTime)
                {
                    firstTime = false;
                }
                else
                {
                    executed = true;
                }
            }
        }
    }
};
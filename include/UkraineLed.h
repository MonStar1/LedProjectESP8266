#include "BaseLed.h"
#include "LedConfig.h"
#include "LogDebug.h"

CRGB blue = CRGB(188, 67, 0);
CRGB orange = CRGB(0, 204, 51);

class UkraineLed : public BaseLed
{
private:
    bool isInitialized = false;

public:
    void loop()
    {
        if (!isInitialized)
        {
            for (int i = 0; i < NUM_LEDS; i++)
            {
                if (i % 2 == 0)
                {
                    leds[i] = blue;
                } else {
                    leds[i] = orange;
                }
            }

            isInitialized = true;
            FastLED.show();
        }
    }
};
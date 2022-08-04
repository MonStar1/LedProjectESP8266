#include "BaseLed.h"
#include "LedConfig.h"
#include "LogDebug.h"

class LoadingLed : public BaseLed
{
private:
    int lightLedsCount = 1;

public:
    void loop()
    {
        EVERY_N_MILLISECONDS(100)
        {
            for (int i = 0; i < lightLedsCount; i++)
            {
                leds[i] = CRGB::White;
            }
            lightLedsCount = lightLedsCount + 1;
            debugD("lightLedsCount: %d", lightLedsCount);
            if (lightLedsCount == NUM_LEDS)
            {
                lightLedsCount = 1;
            }
        }
        FastLED.show();
    }
};
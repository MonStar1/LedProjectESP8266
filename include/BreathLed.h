#include "BaseLed.h"
#include "LedConfig.h"

class BreathLed : public BaseLed
{
private:
public:
    void loop()
    {
        int x = beatsin8(5);
        fill_noise8(leds, NUM_LEDS, 1, x, 10, 1, 100, 10, millis() / 10);
        FastLED.show();
    }
};
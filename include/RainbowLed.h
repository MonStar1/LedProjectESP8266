#include "BaseLed.h"
#include "LedConfig.h"

class RainbowLed : public BaseLed
{
private:
public:
    void loop()
    {
        fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
        FastLED.show();
    }
};
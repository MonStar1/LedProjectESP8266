#include "BaseLed.h"
#include "LedConfig.h"

DEFINE_GRADIENT_PALETTE(heatmap_gp){
    0, 0, 255, 100, // yellow
    125, 0, 200, 2, // red
    200, 0, 10, 0,   // black
    255, 0, 0, 0};  // black

class FlameLed : public BaseLed
{
private:
    bool executed = false;
    int scales[NUM_LEDS];

    CRGBPalette16 myPal = heatmap_gp;

public:
    void loop()
    {
        if (!executed)
        {
            executed = true;
            for (int i = 0; i < NUM_LEDS; i++)
            {
                scales[i] = random8();
            }
        }

        // for (int i = 0; i < NUM_LEDS; i++)
        // {
        //     uint8_t brithness = inoise8(i * 10 + millis() / 15);
        //     uint8_t heatindex = inoise8(i * scales[i] + (millis() / 20 + scales[i]));
        //     long index = map(heatindex, 50, 210, 20, 255);
        //     leds[i] = ColorFromPalette(myPal, index, brithness, LINEARBLEND);
        // }

        for (int i = 0; i < NUM_LEDS; i++)
        {
            uint8_t heatindex = inoise8(i * 100, millis() / 15);
            leds[i] = ColorFromPalette(myPal, heatindex, 255, LINEARBLEND);
        }

        FastLED.show();
    }
};
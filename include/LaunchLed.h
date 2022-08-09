#include "BaseLed.h"
#include "LedConfig.h"
#include "LogDebug.h"

class LaunchLed : public BaseLed
{
private:
    int offset1 = NUM_LEDS / 2;
    int offset2 = NUM_LEDS / 2;
    CHSV copyLeds[NUM_LEDS];

    bool setup = true;
    bool step1 = false;
    bool step2 = false;
    int speed = (4 - NUM_LEDS / 50) * 10;

public:
    void loop()
    {
        // setup
        if (setup)
        {
            // fill_rainbow(leds, NUM_LEDS, 0, 255 / NUM_LEDS);
            for (int i = 0; i < NUM_LEDS; i++)
            {
                copyLeds[i] = CHSV(map(i, 0, NUM_LEDS, 0, 255), 255, 255);
                leds[i] = copyLeds[i];
            }
            setup = false;
            step1 = true;
        }

        // step 1
        if (step1)
        {
            EVERY_N_MILLIS(speed)
            {
                for (int i = offset1; i < offset2; i++)
                {
                    leds[i].nscale8(50);
                }

                offset1 = max(0, --offset1);
                offset2 = min(NUM_LEDS, ++offset2);
                debugD("offset1: %d,  offset2: %d", offset1, offset2);

                if (offset2 - offset1 == NUM_LEDS)
                {
                    offset1 = 0;
                    offset2 = NUM_LEDS;
                    step1 = false;
                    step2 = true;
                }
            }
        }

        // step 2
        if (step2)
        {
            EVERY_N_MILLIS(speed)
            {
                for (int i = 0; i < offset1; i++)
                {
                    leds[i] = copyLeds[i];
                }

                for (int i = offset2; i < NUM_LEDS; i++)
                {
                    leds[i] = copyLeds[i];
                }

                offset1 = min(NUM_LEDS / 2, ++offset1);
                offset2 = max(NUM_LEDS / 2, --offset2);
                debugD("offset1: %d,  offset2: %d", offset1, offset2);

                if (offset1 == NUM_LEDS / 2)
                {
                    step2 = false;
                    setup = true;
                }
            }
        }

        FastLED.show();
    }
};
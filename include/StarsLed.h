#include "BaseLed.h"
#include "LedConfig.h"

#define NUMBER_OF_STARS 40
struct Light
{
    long timebase = 0;
    int position = 0;
    int sin = 0;
};

class StarsLed : public BaseLed
{
private:
    Light lights[NUMBER_OF_STARS];

    int count = 1;

    int getPosition(int position)
    {
        for (Light light : lights)
        {
            if (light.position == position)
            {
                return getPosition(map(random8(), 0, 255, 0, NUM_LEDS));
            }
        }

        return position;
    }

    int beats_per_minute = 3;
    int max_rang_ms = 60 / beats_per_minute * 1000;

    void runLight(Light *light)
    {
        unsigned long mil = millis();
        if (light->timebase == 0 || (mil - light->timebase) >= max_rang_ms)
        {
            light->timebase = mil;
            light->position = getPosition(map(random8(), 0, 255, 0, NUM_LEDS));
        }

        light->sin = beatsin8(beats_per_minute, 0, 255, light->timebase, 190);
        leds[light->position] = CHSV(0, 0, light->sin);
    }

public:
    void loop()
    {
        fadeToBlackBy(leds, NUM_LEDS, 1);

        for (int i = 0; i < count; i++)
        {
            runLight(&lights[i]);
        }

        EVERY_N_MILLISECONDS(1300)
        {
            if (count < NUMBER_OF_STARS)
            {
                count++;
            }
        }

        FastLED.show();
    }
};
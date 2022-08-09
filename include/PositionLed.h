#include "BaseLed.h"
#include "LedConfig.h"
#include "LogDebug.h"

class PositionLed : public BaseLed
{
private:
    bool executed = false;
    int start = 0;
    int end = NUM_LEDS;

    int position = 0;

public:
    PositionLed(int start, int end)
    {
        this->start = min(start, NUM_LEDS);
        this->end = min(end, NUM_LEDS);
        position = this->end;

        clearLed();
    }

    void loop()
    {
        fadeToBlackBy(leds, position, 20);

        if (!executed)
        {
            EVERY_N_MILLISECONDS(100)
            {
                position--;
                debugD("position: %d", position);
                debugD("start: %d", start);
                debugD("end: %d", end);
            }

            if (position == start)
            {
                executed = true;
            }
            else
            {
                int x = map(beat8(240, 0), 0, 255, 0, end);
                leds[x] = CRGB::White;
            }
        }

        FastLED.show();
    }
};
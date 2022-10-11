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
            executed = true;
            clearLed();
            FastLED.show();
        }

        // EVERY_N_SECONDS(5)
        // {
        //     if (firstTime == false)
        //     {
        //         executed = true;
        //         clearLed();
        //         FastLED.show();
        //     }
        //     else
        //     {
        //         firstTime = false;
        //     }
        // }
    }
};
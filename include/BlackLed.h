#include "BaseLed.h"
#include "LedConfig.h"

class BlackLed : public BaseLed
{
private:
    bool executed = false;

public:
    void loop()
    {
        if (!executed)
        {
            executed = true;
            clearLed();
            FastLED.show();
        }
    }
};
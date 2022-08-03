#include "BaseLed.h"
#include "LedConfig.h"

class BlackLed : public BaseLed
{
private:
public:
    void loop()
    {
        clearLed();
        FastLED.show();
    }
};
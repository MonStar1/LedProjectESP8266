#ifndef LOGDEBUG_H
#define LOGDEBUG_H
#include <FastLED.h>
#include "BaseClass.h"
#include "RemoteDebug.h" //https://github.com/JoaoLopesF/RemoteDebug

RemoteDebug Debug;

class LogDebug : public BaseClass
{
    void setup()
    {
    }
    void setupConnected()
    {
        Debug.begin("myesp8266");
        Debug.setResetCmdEnabled(true); // Enable the reset command
        Debug.setSerialEnabled(true);
    }
    void loop()
    {
    }
    void loopConnected()
    {
        Debug.handle();
    }
};

#endif // LOGDEBUG_H
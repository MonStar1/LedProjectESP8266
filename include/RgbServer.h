#include "BaseClass.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

class RgbServer : public BaseClass
{
public:
    ESP8266WebServer server();

    void setup()
    {
    }
    void setupConnected()
    {
    }
    void loop()
    {
    }
    void loopConnected()
    {
    }
};
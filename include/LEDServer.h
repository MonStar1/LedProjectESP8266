#include "BaseClass.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "LogDebug.h"

class LEDServer : public BaseClass
{
public:
    ESP8266WebServer s = ESP8266WebServer(180);

    void handleRoot()
    {
        debugD("handleRoot");
        s.send(200, "text/plain", "OK");
    }
    void setup()
    {
    }
    void setupConnected()
    {
        s.on("/", [&]
             { handleRoot(); });

        s.begin();
    }
    void loop()
    {
    }
    void loopConnected()
    {
        s.handleClient();
    }
};
#include "BaseClass.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "LogDebug.h"
#include "LedConfig.h"
#include "Button2.h"

#define BUTTON_PIN D4
#define LED_PIN D7

class LEDServer : public BaseClass
{
public:
    LEDServer(std::function<void(int)> modeCallback)
    {
        this->modeCallback = modeCallback;
    };
    void setup()
    {
        EEPROM.begin(6);
        readEEPROM();
        showColor();
        pinMode(LED_PIN, OUTPUT);
        digitalWrite(LED_PIN, HIGH);
        button.begin(BUTTON_PIN, INPUT_PULLUP);
        button.setClickHandler([&](Button2 &btn)
                               { switchLed(); });

        button.setDoubleClickHandler([&](Button2 &btn)
                                     { doubleClick(); });

        button.setLongClickHandler([&](Button2 &btn)
                                   { doubleClick(); });
    }
    void setupConnected()
    {
        server.on("/", [&]()
                  { handleRoot(); });
        server.on("/status", [&]()
                  { status(); });
        server.on("/setBrightness", [&]()
                  { setBrightness(); });
        server.on("/getBrightness", [&]()
                  { getBrightness(); });
        server.on("/getHue", [&]()
                  { getHue(); });
        server.on("/setHue", [&]()
                  { setHue(); });
        server.on("/setSaturation", [&]()
                  { setSaturation(); });
        server.on("/getSaturation", [&]()
                  { getSaturation(); });
        server.on("/ledMode", [&]()
                  { 
                    turnOff();
                    currentMode = server.arg("mode").toInt();
                    modeCallback(currentMode);
                    server.send(200, "text", "OK"); });

        server.on("/ledStatus", [&]()
                  { ledStatus(); });

        server.begin();
    }
    void loop()
    {
        EVERY_N_SECONDS(1)
        {
            timer++;
            if (changeTime != 0 && timer - changeTime > 10)
            {
                writeEEPROM();
                changeTime = 0;
                timer = 0;
            }
        }

        showColor();

        button.loop();
    }

    void loopConnected()
    {
        server.handleClient();
    }

private:
    ESP8266WebServer server = ESP8266WebServer(80);

    Button2 button;

    bool swch;
    CHSV color = CHSV(0, 0, 255);

    int timer = 0;
    int changeTime = 0;

    std::function<void(int)> modeCallback;

    uint8_t currentMode = 255;

    void setMode(int mode)
    {
        turnOff();
        currentMode = mode;
        modeCallback(currentMode);
    }

    void ledStatus()
    {
        int mode = server.arg(0).toInt();
        debugD("ledStatus: %d", mode);
        if (mode == currentMode)
        {
            server.send(200, "text/plain", "1");
        }
        else
        {
            server.send(200, "text/plain", "0");
        }
    }

    void turnOff()
    {
        swch = false;
        currentMode = 255;
        fill_solid(leds, NUM_LEDS, CHSV(0, 0, 0));
        FastLED.show();
    }

    void handleRoot()
    {
        for (uint8_t i = 0; i < server.args(); i++)
        {
            String argName = server.argName(i);
            String argValue = server.arg(i);
            if (argName == "switch")
            {
                swch = argValue == "ON";
            }
        }

        if (!swch)
        {
            fill_solid(leds, NUM_LEDS, CHSV(0, 0, 0));
            FastLED.show();
        }
        else
        {
            currentMode = 255;
            modeCallback(255);
        }

        server.send(200, "text/plain", "OK");

        changeTime = timer;
    }

    void switchLed()
    {
        debugD("button clicked");
        swch = !swch;

        if (swch)
        {
            color = CHSV(0, 0, 255);
            fill_solid(leds, NUM_LEDS, CHSV(0, 0, 0));
            FastLED.show();
        }
        else
        {
            currentMode = 255;
            modeCallback(255);
        }

        changeTime = timer;
    }

    void doubleClick()
    {
        debugD("button double clicked");
        setMode(++currentMode);
    }

    void status()
    {
        int status = 0;
        if (swch)
        {
            status = 1;
        }
        else
        {
            status = 0;
        }
        server.send(200, "text/plain", String(status));
    }

    void setBrightness()
    {
        for (uint8_t i = 0; i < server.args(); i++)
        {
            String argName = server.argName(i);
            String argValue = server.arg(i);
            if (argName == "brightness")
            {
                FastLED.setBrightness(map(argValue.toInt(), 0, 100, 1, 255));
            }
        }

        server.send(200, "text/plain", "OK");
        debugD("value = %d", FastLED.getBrightness());

        changeTime = timer;
        printHSV();
    }

    void getBrightness()
    {
        String value = String(map(FastLED.getBrightness(), 0, 255, 0, 100));
        server.send(200, "text/plain", value);
        debugD("value = %s", value);
    }

    void setHue()
    {
        for (uint8_t i = 0; i < server.args(); i++)
        {
            String argName = server.argName(i);
            String argValue = server.arg(i);
            if (argName == "hue")
            {
                color.h = (int)map(argValue.toInt(), 0, 360, 0, 255);
            }
        }

        server.send(200, "text/plain", "OK");
        debugD("hue = %d", color.hue);

        changeTime = timer;
        printHSV();
    }

    void getHue()
    {
        String value = String(color.hue * 360 / 255);
        server.send(200, "text/plain", value);
        debugD("hue = %s", value);
    }

    void setSaturation()
    {
        for (uint8_t i = 0; i < server.args(); i++)
        {
            String argName = server.argName(i);
            String argValue = server.arg(i);
            if (argName == "saturation")
            {
                color.s = map(argValue.toInt(), 0, 100, 0, 255);
            }
        }

        server.send(200, "text/plain", "OK");
        debugD("sat = %d", color.sat);

        changeTime = timer;
        printHSV();
    }

    void getSaturation()
    {
        String value = String(color.sat * 100 / 255);
        server.send(200, "text/plain", value);
        debugD("sat = %s", value);
    }

    void showColor()
    {
        if (swch)
        {
            fill_solid(leds, NUM_LEDS, color);
            FastLED.show();
        }
    }

    void printHSV()
    {
        debugD("HSV: h=%d s=%d v=%d", color.h, color.s, color.v);
    }

    void writeEEPROM()
    {
        EEPROM.write(0, swch);
        EEPROM.write(1, color.h);
        EEPROM.write(2, color.s);
        EEPROM.write(3, FastLED.getBrightness());

        EEPROM.commit();
    }

    void readEEPROM()
    {
        swch = EEPROM.read(0);
        color.h = EEPROM.read(1);
        color.s = EEPROM.read(2);
        FastLED.setBrightness(EEPROM.read(3));

        if (swch)
        {
            currentMode = 255;
            modeCallback(255);
        }
    }
};
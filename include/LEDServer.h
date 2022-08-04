#include "BaseClass.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "LogDebug.h"

#define DATA_PIN D7

#define redPin D5
#define greenPin D6
#define bluePin D2

class LEDServer : public BaseClass
{
public:
    // LEDServer(std::function<void(ESP8266WebServer)> ledModeCallback);
    ESP8266WebServer server = ESP8266WebServer(80);
    CHSV hsvColor;
    void setup()
    {
        EEPROM.begin(6);
        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
        pinMode(bluePin, OUTPUT);
        readEEPROM();
        showColor();
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
        // server.on("/ledMode", [&]()
        //           { ledModeCallback(server); });

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
    }
    void loopConnected()
    {
        server.handleClient();
    }

private:
    std::function<void(ESP8266WebServer)> ledModeCallback;
    bool swch;
    CRGB rgbColor;

    int timer = 0;
    int changeTime = 0;

    void handleRoot()
    {
        String message = "File Not Found\n\n";
        message += "URI: ";
        message += server.uri();
        message += "\nMethod: ";
        message += (server.method() == HTTP_GET) ? "GET" : "POST";
        message += "\nArguments: ";
        message += server.args();
        message += "\n";

        for (uint8_t i = 0; i < server.args(); i++)
        {
            String argName = server.argName(i);
            String argValue = server.arg(i);
            if (argName == "switch")
            {
                swch = argValue == "ON";
            }
        }

        message += "R: " + String(rgbColor.red) + " G: " + String(rgbColor.green) + " B: " + String(rgbColor.blue) + " switch: " + String(swch);
        server.send(200, "text/plain", message);

        changeTime = timer;
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
                hsvColor.value = map(argValue.toInt(), 0, 100, 0, 255);
            }
        }

        hsv2rgb_rainbow(hsvColor, rgbColor);

        server.send(200, "text/plain", "OK");
        debugD("value = %d", hsvColor.value);

        changeTime = timer;
        printHSV();
    }

    void getBrightness()
    {
        String value = String(hsvColor.val * 100 / 255);
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
                hsvColor.h = (int)map(argValue.toInt(), 0, 360, 0, 255);
            }
        }

        hsv2rgb_spectrum(hsvColor, rgbColor);

        server.send(200, "text/plain", "OK");
        debugD("hue = %d", hsvColor.hue);

        changeTime = timer;
        printHSV();
    }

    void getHue()
    {
        String value = String(hsvColor.hue * 360 / 255);
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
                hsvColor.s = map(argValue.toInt(), 0, 100, 0, 255);
            }
        }

        hsv2rgb_rainbow(hsvColor, rgbColor);

        server.send(200, "text/plain", "OK");
        debugD("sat = %d", hsvColor.sat);

        changeTime = timer;
        printHSV();
    }

    void getSaturation()
    {
        String value = String(hsvColor.sat * 100 / 255);
        server.send(200, "text/plain", value);
        debugD("sat = %s", value);
    }

    // prepare a web page to be send to a client (web browser)
    String prepareHtmlPage()
    {
        String htmlPage;
        htmlPage.reserve(1024); // prevent ram fragmentation
        htmlPage = F("HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "Connection: close\r\n" // the connection will be closed after completion of the response
                     "Refresh: 5\r\n"        // refresh the page automatically every 5 sec
                     "\r\n"
                     "<!DOCTYPE HTML>"
                     "<html>"
                     "Analog input:  ");
        htmlPage += F("</html>"
                      "\r\n");
        return htmlPage;
    }

    void showColor()
    {
        if (swch)
        {
            analogWrite(redPin, 255 - rgbColor.red);
            analogWrite(greenPin, 255 - rgbColor.green);
            analogWrite(bluePin, 255 - rgbColor.blue);
        }
        else
        {
            analogWrite(redPin, 255);
            analogWrite(greenPin, 255);
            analogWrite(bluePin, 255);
        }
    }

    void printRGB()
    {
        debugD("RGB: r=%d g=%d b=%d", rgbColor.red, rgbColor.green, rgbColor.blue);
    }

    void printHSV()
    {
        debugD("HSV: h=%d s=%d v=%d", hsvColor.h, hsvColor.s, hsvColor.v);
        printRGB();
    }

    void writeEEPROM()
    {
        EEPROM.write(0, swch);
        EEPROM.write(1, hsvColor.h);
        EEPROM.write(2, hsvColor.s);
        EEPROM.write(3, hsvColor.v);

        EEPROM.commit();
    }

    void readEEPROM()
    {
        swch = EEPROM.read(0);
        hsvColor.h = EEPROM.read(1);
        hsvColor.s = EEPROM.read(2);
        hsvColor.v = EEPROM.read(3);

        hsv2rgb_rainbow(hsvColor, rgbColor);
    }
};

// LEDServer::LEDServer(std::function<void(ESP8266WebServer)> ledModeCallback)
// {
//     this->ledModeCallback = ledModeCallback;
// }
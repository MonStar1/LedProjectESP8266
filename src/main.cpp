#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <memory>
#include <vector>
#include "MyNewFile.h"
#include "Ota.h"
#include "LEDServer.h"
#include "Plotter.h"
#include "LogDebug.h"

using namespace std;

ESP8266WebServer server(80);

// Plotted variables must be declared as globals
double x;
double y;

// Also declare plotter as global
Plotter p;

#define NUM_LEDS 1
#define DATA_PIN D7

#define redPin D5
#define greenPin D6
#define bluePin D2

CRGB leds[NUM_LEDS];

enum WiFiStatus
{
  NONE,
  CONNECTING,
  CONNECTED,
  WORK
};

vector<unique_ptr<BaseClass>> lifecycle;

bool swch;
CRGB rgbColor;
CHSV hsvColor;

Ticker tick;
int timer = 0;
int changeTime = 0;

WiFiStatus wfStatus = NONE;

void ticker()
{
  ++timer;
  // debugD("Tick: " + String(timer));
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

void animateDagger()
{
  for (int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1)
  {
    // Turn our current led on to white, then show the leds
    leds[whiteLed] = CRGB::White;

    // Show the leds (only one of which is set to white, from above)
    FastLED.show();

    // Wait a little bit
    delay(20);
  }
}

void setupWiFi()
{
  if (wfStatus == NONE)
  {
    debugD();

    WiFi.begin("KoPoHa", "07760776");
    debugD("Connecting");
    wfStatus = CONNECTING;
  }
  else if (wfStatus == CONNECTING)
  {
    digitalWrite(LED_BUILTIN, LOW);
    if (WiFi.status() == WL_CONNECTED)
    {
      wfStatus = CONNECTED;
    }
  }
  else if (wfStatus == CONNECTED)
  {
    digitalWrite(LED_BUILTIN, HIGH);

    debugD("Connected, IP address: %s", WiFi.localIP().toString());

    server.on("/", handleRoot);
    server.on("/status", status);
    server.on("/setBrightness", setBrightness);
    server.on("/getBrightness", getBrightness);
    server.on("/getHue", getHue);
    server.on("/setHue", setHue);
    server.on("/setSaturation", setSaturation);
    server.on("/getSaturation", getSaturation);
    server.on("/blink", startBlink);

    server.begin();
    
    wfStatus = WORK;

    for (auto const &item : lifecycle)
    {
      item.get()->setupConnected();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(6);
  readEEPROM();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  showColor();

  // FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  // GRB ordering is typical

  tick.attach(1, ticker);

  // Start plotter
  p.Begin();

  // Add time graphs. Notice the effect of points displayed on the time scale
  p.AddTimeGraph("HSV graph", 1000, "Hue", hsvColor.h, "Sat", hsvColor.s, "Val", hsvColor.v);

  lifecycle.push_back(make_unique<LogDebug>());
  lifecycle.push_back(make_unique<OtaUpdate>());
  lifecycle.push_back(make_unique<LEDServer>());

  for (auto const &item : lifecycle)
  {
    item.get()->setup();
  }
}

void loop()
{
  // Update variables with arbitrary sine/cosine data
  // x = map(beat8(6, 0), 0, 255, 0, NUM_LEDS);

  // hsvColor.h = beat8(2, 0);
  // hsvColor.v = beatsin8(20);
  // hsv2rgb_rainbow(hsvColor, rgbColor);

  // // Plot
  // p.Plot();

  // EVERY_N_SECONDS(1) {}

  // hsv2rgb_rainbow(hsvColor, rgbColor);

  setupWiFi();
  if (wfStatus == WORK)
  {
    server.handleClient();

    for (auto const &item : lifecycle)
    {
      item.get()->loopConnected();
    }
  }

  showColor();

  if (changeTime != 0 && timer - changeTime > 10)
  {
    writeEEPROM();
    changeTime = 0;
    timer = 0;
  }

  for (auto const &item : lifecycle)
  {
    item.get()->loop();
  }
}
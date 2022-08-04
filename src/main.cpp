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
#include "BaseLed.h"
#include "DaggerLed.h"
#include "LedConfig.h"
#include "RainbowLed.h"
#include "BlackLed.h"
#include "LaunchLed.h"
#include "LoadingLed.h"
#include "SinLed.h"

using namespace std;

// Also declare plotter as global
Plotter p;

int ledMode = 0;

unique_ptr<BaseLed> baseLed;

enum WiFiStatus
{
  NONE,
  CONNECTING,
  CONNECTED,
  WORK
};

vector<unique_ptr<BaseClass>> lifecycle;

WiFiStatus wfStatus = NONE;

void setupWiFi()
{
  if (wfStatus == NONE)
  {
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

    wfStatus = WORK;

    for (auto const &item : lifecycle)
    {
      item.get()->setupConnected();
    }
  }
}

void ledServerCallback(ESP8266WebServer server)
{
}
void setup()
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  setupLed();

  // Start plotter
  p.Begin();

  // Add time graphs. Notice the effect of points displayed on the time scale
  // p.AddTimeGraph("HSV graph", 1000, "Hue", hsvColor.h, "Sat", hsvColor.s, "Val", hsvColor.v);

  lifecycle.push_back(make_unique<LogDebug>());
  lifecycle.push_back(make_unique<OtaUpdate>());
  lifecycle.push_back(make_unique<LEDServer>());

  for (auto const &item : lifecycle)
  {
    item.get()->setup();
  }

  baseLed = make_unique<BlackLed>();
}

void loop()
{
  setupWiFi();
  if (wfStatus == WORK)
  {
    for (auto const &item : lifecycle)
    {
      item.get()->loopConnected();
    }
  }

  for (auto const &item : lifecycle)
  {
    item.get()->loop();
  }

  // EVERY_N_SECONDS(5)
  // {
  //   clearLed();
  //   switch (ledMode++ % 3)
  //   {
  //   case 0:
  //     baseLed = make_unique<RainbowLed>();
  //     break;
  //   case 1:
  //     baseLed = make_unique<DaggerLed>();
  //     break;
  //   default:
  //     baseLed = make_unique<BlackLed>();
  //     break;
  //   }
  // }

  baseLed.get()->loop();
}
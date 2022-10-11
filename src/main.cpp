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
#include "PositionLed.h"
#include "BreathLed.h"
#include "StarsLed.h"
#include "FlameLed.h"
#include "UkraineLed.h"

using namespace std;

// Also declare plotter as global
Plotter p;

int ledMode = 0;

shared_ptr<BaseLed> baseLed = make_shared<BlackLed>();

enum WiFiStatus
{
  NONE,
  CONNECTING,
  CONNECTED,
  WORK
};

vector<shared_ptr<BaseClass>> lifecycle;

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

void setMode(int mode)
{
  debugD("setMode: %d", mode);

  ledMode = mode;
  clearLed();

  switch (ledMode)
  {
  case 0:
    baseLed = make_shared<BreathLed>();
    break;
  case 1:
    baseLed = make_shared<FlameLed>();
    break;
  case 2:
    baseLed = make_shared<PositionLed>(30, NUM_LEDS - 1);
    break;
  case 3:
    baseLed = make_shared<LoadingLed>();
    break;
  case 4:
    baseLed = make_shared<SinLed>();
    break;
  case 5:
    baseLed = make_shared<LaunchLed>();
    break;
  case 6:
    baseLed = make_shared<RainbowLed>();
    break;
  case 7:
    baseLed = make_shared<StarsLed>();
    break;
  case 8:
    baseLed = make_shared<DaggerLed>();
    break;
  case 9:
    baseLed = make_shared<UkraineLed>();
    break;
  default:
    baseLed = make_shared<BlackLed>();
    break;
  }
}

shared_ptr<LEDServer> ledServer = make_shared<LEDServer>(setMode);

void setup()
{
  Serial.begin(115200);
  setupLed();

  // Start plotter
  p.Begin();

  // Add time graphs. Notice the effect of points displayed on the time scale
  // p.AddTimeGraph("HSV graph", 1000, "Hue", hsvColor.h, "Sat", hsvColor.s, "Val", hsvColor.v);

  lifecycle.push_back(make_shared<LogDebug>());
  lifecycle.push_back(make_shared<OtaUpdate>());
  lifecycle.push_back(ledServer);

  for (auto const &item : lifecycle)
  {
    item.get()->setup();
  }
}

void loop()
{
  if (wfStatus == WORK)
  {
    for (auto const &item : lifecycle)
    {
      item.get()->loopConnected();
    }
  }
  else
  {
    setupWiFi();
  }

  for (auto const &item : lifecycle)
  {
    item.get()->loop();
  }

  baseLed.get()->loop();
}
#include <Ticker.h>
#include <ESP8266WiFi.h>

Ticker blnk;
int blinkCounter = 0;

void blink()
{
  blinkCounter += 1;
  int value = 0;
  if (blinkCounter % 2 == 0)
  {
    value = LOW;
  }
  else
  {
    value = HIGH;
  };
  digitalWrite(LED_BUILTIN, value);
  if (blinkCounter == 50)
  {
    blinkCounter = 0;
    blnk.detach();
    digitalWrite(LED_BUILTIN, HIGH);
  }
}

void startBlink()
{
  blnk.attach_ms(100, blink);
}

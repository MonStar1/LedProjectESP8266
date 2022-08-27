#include <FastLED.h>

#ifndef LEDCONFIG_H
#define LEDCONFIG_H

#define NUM_LEDS 69
#define DATA_PIN D1

CRGB leds[NUM_LEDS];

void clearLed()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CRGB::Black;
    }
}

void setupLed()
{
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS); // GRB ordering is typical
    FastLED.setBrightness(255);
    clearLed();
    FastLED.show();
}

#endif // LEDCONFIG_H
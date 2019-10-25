#include <FastLED.h>

#define NUM_PIXELS 21
#define PIXEL_PIN 4
#define BRIGHT_MAX 10

CRGB strip[NUM_PIXELS];

uint32_t COLOUR_OFF = CRGB::Black;
uint32_t COLOUR_WHITE = CRGB::White;
uint32_t COLOUR_RED = CRGB::Red;
uint32_t COLOUR_DARKRED = CRGB::DarkRed;
uint32_t COLOUR_GREEN = CRGB::Green;

void allLedsOn(uint32_t colour)
{
  for (int i = 0; i < NUM_PIXELS; i++)
  {
    strip[i] = colour;
  }
  FastLED.show();
}

void fadeall()
{
  for (int i = 0; i < NUM_PIXELS; i++)
  {
    strip[i].nscale8(250);
  }
}

void initialiseLeds(uint32_t colour)
{
  FastLED.addLeds<WS2812B, PIXEL_PIN, GRB>(strip, NUM_PIXELS);
  FastLED.setBrightness(50);
  allLedsOn(colour);
  FastLED.show();
}

void drawBatteryPercent(uint8_t percent)
{
  uint8_t map = NUM_PIXELS * ((percent*1.0)/100);
  // Serial.printf("pc: %d map: %d\n", percent, map);
  for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    if (i <= map) {
      strip[i] = COLOUR_GREEN;
    }
    else {
      strip[i] = COLOUR_OFF;
    }
  }
  FastLED.show();
}
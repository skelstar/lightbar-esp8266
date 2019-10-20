#include <Arduino.h>
#include <FastLED.h>

#define NUM_PIXELS  21
#define PIXEL_PIN   4
#define BRIGHT_MAX  10

CRGB strip[NUM_PIXELS];

uint32_t COLOUR_OFF = CRGB::Black;
uint32_t COLOUR_WHITE = CRGB::White;
uint32_t COLOUR_RED = CRGB::Red;


void allLedsOn(uint32_t colour) {
  for (int i=0; i<NUM_PIXELS; i++) {
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

void initialiseLeds() {
  FastLED.addLeds<WS2812B, PIXEL_PIN, GRB>(strip, NUM_PIXELS);
  FastLED.setBrightness(50);
  allLedsOn(COLOUR_WHITE);
  FastLED.show();
}

void setup() {

  initialiseLeds();
}

void loop() {
  delay(10);
}

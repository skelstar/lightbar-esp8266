#include <Arduino.h>

#include "leds.h"

void setup() {
  Serial.begin(115200);
  Serial.printf("\n\nready!\n");

  initialiseLeds();
}

void loop() {
  delay(10);
}

#include <Arduino.h>
#include "LED.h"

LED statusLED(5, true); // GPIO5

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("LED Beispiel: Blink nicht-blockierend");
  statusLED.begin();

  // Starte Blink: 200ms an, 800ms aus
  statusLED.blink(200, 800);
}

void loop() {
  // update muss regelmäßig aufgerufen werden
  statusLED.update();

  // Beispiel: bei Tastendruck toggle
  // if (digitalRead(BUTTON_PIN) == LOW) { statusLED.toggle(); }
}

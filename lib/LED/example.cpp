/**
 * Beispiel zur Nutzung der LED-Bibliothek
 */

#include <Arduino.h>
#include "LED.h"

LED statusLED(5, true); // GPIO5

void setup() {
    Serial.begin(115200);
    delay(50);
    Serial.println("LED Beispiel: Blink nicht-blockierend");
    statusLED.begin();
    statusLED.blink(200, 800);
}

void loop() {
    statusLED.update();
}

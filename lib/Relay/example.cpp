#include <Arduino.h>
#include "Relay.h"

// Beispiel für ein einzelnes Relais an Pin 14 (anpassen)
Relay pumpRelay(25, false, false);  // GPIO25, activeHigh=false, safeState=false

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("Relay Beispiel: Beginne...");
  pumpRelay.begin();

  Serial.println("Schalte Relais kurz 2s ein (Pulse)...");
  pumpRelay.pulse(2000); // nicht-blockierender Pulse 2 Sekunden
}

void loop() {
  // Wichtiger Aufruf: update verwaltet Pulse-Timeout
  pumpRelay.update();

  // Demo: bei serieller Eingabe toggle
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 't') {
      pumpRelay.toggle();
      Serial.print("Toggled, neuer Zustand: ");
      Serial.println(pumpRelay.isOn() ? "ON" : "OFF");
    } else if (c == 'o') {
      pumpRelay.on();
      Serial.println("Relais ON");
    } else if (c == 'f') {
      pumpRelay.off();
      Serial.println("Relais OFF");
    } else if (c == 'p') {
      pumpRelay.pulse(1000);
      Serial.println("Pulse 1s gestartet");
    }
  }

  delay(10); // kleines Sleep, update bleibt responsiv
}

/**
 * @file example.cpp
 * @brief Beispiel zur Nutzung der SensorXKCY25NPN-Klasse.
 */

#include <Arduino.h>
#include "SensorXKCY25NPN.h"

SensorXKCY25NPN levelSensor(35, false); // GPIO35 (nur als Eingangs-Pin geeignet)

void setup() {
    Serial.begin(115200);
    delay(50);
    Serial.println("Starte XKC-Y25-NPN Test...");
    levelSensor.begin();
    Serial.println(levelSensor.wiringNotes());
}

void loop() {
    if (levelSensor.read()) {
        Serial.println(">WaterDetected:1"); // Serial Plotter / Parser kompatibel
    } else {
        Serial.println(">WaterDetected:0");
    }
    delay(1000);
}

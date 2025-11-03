/**
 * @file example.cpp
 * @brief Beispiel zur Nutzung der DS18B20-Klasse.
 * 
 * Das Beispiel liest alle zwei Sekunden die Bodentemperatur vom Sensor und sendet sie über die serielle Schnittstelle. 
 * 
 * Ein [Serial Plotter](https://github.com/badlogic/serial-plotter) könnte die Daten entgegennehmen und visualisieren.
 */

#include <Arduino.h>
#include "DS18B20.h"

DS18B20 sensor(4); // GPIO4

void setup() {
    Serial.begin(115200);
    Serial.println("Starte DS18B20-Test...");
}

void loop() {
    if (sensor.read()) {
        Serial.print(">");
        Serial.print("Temperatur:"); // °C
        Serial.print(sensor.getTemperature());
        Serial.println();
    } else {
        Serial.print("Fehler ");
        Serial.print(sensor.getLastError());
        Serial.print(": ");
        Serial.println(sensor.getErrorMessage());
    }
    delay(2000);
}

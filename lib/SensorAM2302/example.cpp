/**
 * @file example.cpp
 * @brief Beispiel zur Nutzung der SensorAM2302-Klasse.
 * 
 * Das Beispiel liest alle zwei Sekunden die Raumtemperatur und die Luftfeuchtigkeit vom Sensor und sendet sie über die serielle Schnittstelle. 
 * 
 * Ein [Serial Plotter](https://github.com/badlogic/serial-plotter) könnte die Daten entgegennehmen und visualisieren.
 */

#include <Arduino.h>
#include "SensorAM2302.h"

SensorAM2302 sensor(13); // GPIO13

void setup() {
    Serial.begin(115200);
    Serial.println("Starte SensorAM2302-Test...");
}

void loop() {
    if (sensor.read()) {
        Serial.print(">");
        Serial.print("Temperatur:"); // °C
        Serial.print(sensor.getTemperature());

        Serial.print(",");
        Serial.print("Luftfeuchtigkeit:"); // Prozent
        Serial.print(sensor.getHumidity());

        Serial.println();
    } else {
        Serial.print("Fehler ");
        Serial.print(sensor.getLastError());
        Serial.print(": ");
        Serial.println(sensor.getErrorMessage());
    }
    delay(2000); 
}

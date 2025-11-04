/**
 * @file example.cpp
 * @brief Beispiel zur Nutzung der SensorBH1750-Klasse.
 *
 * Liest periodisch (1s) die Beleuchtungsstärke in Lux und sendet sie über die serielle Schnittstelle.
 * Format kompatibel mit Serial Plotter (">Label:Wert,Label2:Wert").
 */

#include <Arduino.h>
#include "SensorBH1750.h"

SensorBH1750 light; // Standard-I2C-Adresse 0x23

void setup() {
    Serial.begin(115200);
    delay(50);
    Serial.println("Starte SensorBH1750 Test...");
    Wire.begin(); // Default SDA/SCL Pins benutzen
    if (!light.begin()) {
        Serial.print("Initialisierung fehlgeschlagen: ");
        Serial.println(light.getErrorMessage());
    }
}

void loop() {
    if (light.read()) {
        float lux = light.getLux();
        Serial.print(">");
        Serial.print("LightLux:");
        Serial.print(lux);
        Serial.println();
    } else {
        Serial.print("Fehler ");
        Serial.print(static_cast<int>(light.getLastError()));
        Serial.print(": ");
        Serial.println(light.getErrorMessage());
    }
    delay(1000);
}

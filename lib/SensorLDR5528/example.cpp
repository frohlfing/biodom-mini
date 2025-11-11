/**
 * Beispiel zur Nutzung der SensorLDR5528-Bibliothek
 */

#include <Arduino.h>
#include "SensorLDR5528.h"

#if defined(ARDUINO_ARCH_ESP32)
    #define ADC_MAX_VALUE 4095
#else
    #define ADC_MAX_VALUE 1023
#endif

SensorLDR5528 sensor(36, 10000.0f, ADC_MAX_VALUE); // GPIO36 (ADC1_0 auf ESP32)

void setup() {
    Serial.begin(115200);
    Serial.println("Initialisiere Sensor...");
    if (!sensor.begin()) {
        Serial.print("Initialisierung fehlgeschlagen: ");
        Serial.println(sensor.getErrorMessage());
    }
}

void loop() {
    if (sensor.read()) {
        Serial.print(">Raw:");
        Serial.print(sensor.getRaw());
        Serial.print(",Ohm:");
        Serial.print(sensor.getResistance());
        Serial.print(",Lux:");
        Serial.println(sensor.getLux());
    } else {
        Serial.print("Fehler ");
        Serial.print(sensor.getLastError());
        Serial.print(": ");
        Serial.println(sensor.getErrorMessage());
    }
    delay(2000);
}

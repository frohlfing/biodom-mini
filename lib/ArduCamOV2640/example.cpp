/**
 * Beispiel zur Nutzung der ArduCamOV2640-Bibliothek.
 */

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "ArduCamOV2640.h"

// GPIO-Pin für den SPI Chip Select der SD-Karte und Kamera
constexpr uint8_t SD_CS_PIN = 16;
constexpr uint8_t CAM_CS_PIN = 17;

// Erstelle eine Instanz der Bibliotheksklasse
ArduCamOV2640 camera(CAM_CS_PIN);

void setup() {
    Serial.begin(115200);
    while (!Serial.available()) {
        delay(100);
    }
    Serial.println("Starte ArduCAM-Funktionstest-Beispiel...");

    // I2C-Bus starten
    Wire.begin();

    // Alle CS-Pins auf HIGH setzen und SPI-Bus initialisieren
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    pinMode(CAM_CS_PIN, OUTPUT);
    digitalWrite(CAM_CS_PIN, HIGH);
    SPI.begin();

    // SD-Karten-Initialisierung
    Serial.println(F("Initializing SD card..."));
    while (!SD.begin(SD_CS_PIN)) {
        Serial.println(F("SD Card Error! Retrying..."));
        delay(1000);
    }
    Serial.println(F("SD Card detected."));

    // Kamera initialisieren
    Serial.print("Initialisiere Kamera... ");
    if (!camera.begin()) {
        Serial.println("Kamera nicht gefunden. Bitte Verkabelung prüfen. Programm wird angehalten.");
        return;
    }
    Serial.println("erfolgreich!");

    // TEST 1: Kleines Bild speichern
    Serial.println("Mache Testbild (320x240)...");
    camera.setResolution(OV2640_320x240);
    if (camera.saveToSD("/small.jpg")) {
        Serial.println("Gespeichert: /small.jpg");
    } else {
        Serial.println("Fehler beim Speichern!");
    }

    // TEST 2: Großes Bild speichern (Resolution Change Test)
    Serial.println("Mache HD Bild (1600x1200)...");
    camera.setResolution(OV2640_1600x1200);
    const unsigned long start = millis(); // Bei hoher Auflösung dauert das Speichern deutlich länger
    if (camera.saveToSD("/hd.jpg")) {
        Serial.print("Gespeichert: /hd.jpg in ");
        Serial.print(millis() - start);
        Serial.println(" ms");
    } else {
        Serial.println("Fehler beim Speichern!");
    }

    Serial.println("Test beendet.");
}

void loop() {
    // Bleibt leer, alle Aktionen sind im setup()
}

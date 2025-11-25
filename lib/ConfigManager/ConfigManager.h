#pragma once

#include <Arduino.h>
//#include <ArduinoJson.h>
//#include "config.h" // definiert 'struct ConfigData'

/**
* Verwaltet die zur Laufzeit veränderbare Konfiguration des Systems.
 *
 * Diese Klasse ist verantwortlich für das Laden, Speichern und Bereitstellen von
 * Konfigurationsparametern (z.B. Schwellwerte, Zeitpläne). Die Daten werden
 * persistent als JSON-Datei im LittleFS gespeichert.
 */
class ConfigManager {
public:
    /**
     * @brief Konstruktor.
     * @param filename Der Pfad zur Konfigurationsdatei im Dateisystem.
     */
    explicit ConfigManager(const char* filename = "/config.json");

private:
    /**
     * @property _filename
     * @brief Der Name der Konfigurationsdatei.
     */
    const char* _filename;
};
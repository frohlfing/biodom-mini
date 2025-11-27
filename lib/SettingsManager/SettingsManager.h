#pragma once

#include <ArduinoJson.h>
#include "../../include/settings.h"

/**
 * Verwaltet die zur Laufzeit veränderbaren Einstellungen des Systems.
 *
 * Diese Klasse ist verantwortlich für das Laden, Speichern und Bereitstellen von Einstellungen wie z.B. Schwellwerte
 * oder Zeitpläne. Die Daten werden persistent als JSON-Datei im LittleFS gespeichert.
 */
class SettingsManager {
public:
    /**
     * @brief Konstruktor.
     * @param filename Der Pfad zur JSON-Datei im Dateisystem.
     */
    explicit SettingsManager(const char* filename = "/config.json");

    /**
     * @brief Initialisiert den Manager und lädt die Konfiguration aus dem Dateisystem.
     * @return true bei Erfolg, andernfalls false.
     */
    bool begin();

    /**
     * @brief Speichert die aktuellen Konfigurationsdaten persistent im Dateisystem.
     * @return true bei Erfolg, andernfalls false.
     */
    bool save() const;

    /**
     * @brief Gibt eine schreibgeschützte Referenz auf die Konfigurationsdaten zurück.
     * @return Eine const-Referenz auf die ConfigData-Struktur.
     */
    const Settings& get() const;

    /**
     * @brief Gibt eine beschreibbare Referenz auf die Konfigurationsdaten zurück.
     * Nach der Änderung muss manuell 'save()' aufgerufen werden.
     * @return Eine Referenz auf die ConfigData-Struktur.
     */
    Settings& getMutable();

    /**
     * @brief Füllt ein JSON-Objekt mit den Werten aus der internen _settings-Struktur.
     * @param doc Das zu füllende JsonObject.
     */
    void serialize(const JsonObject& doc) const;

    /**
     * @brief Füllt die interne _settings-Struktur aus einem JSON-Objekt.
     * @param doc Das JsonObject, aus dem die Daten gelesen werden.
     */
    void deserialize(const JsonObject& doc);

private:

    /**
     * @property _settings
     * @brief Die Instanz der ConfigData-Struktur, die die Konfigurationswerte im RAM hält.
     */
    Settings _settings;

    /**
     * @property _filename
     * @brief Der Name der JSON-Datei.
     */
    const char* _filename;
};
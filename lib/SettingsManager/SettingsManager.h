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
     * @param filename Der Pfad zur Konfigurationsdatei im Dateisystem.
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
    bool save() const; // 'const' entfernt, da save() den Zustand nicht ändert, aber oft nach Änderungen aufgerufen wird.

    /**
     * @brief Gibt eine schreibgeschützte Referenz auf die Konfigurationsdaten zurück.
     * @return Eine const-Referenz auf die ConfigData-Struktur.
     */
    const settings& get() const;

    /**
     * @brief Gibt eine beschreibbare Referenz auf die Konfigurationsdaten zurück.
     * Nach der Änderung muss manuell 'save()' aufgerufen werden.
     * @return Eine Referenz auf die ConfigData-Struktur.
     */
    settings& getMutable();

private:
    /**
     * @brief Füllt ein JSON-Dokument mit den Werten aus der internen _config-Struktur.
     * @param doc Das zu füllende JsonDocument.
     */
    void serialize(JsonDocument& doc) const;

    /**
     * @brief Füllt die interne _config-Struktur aus einem JSON-Dokument.
     * @param doc Das JsonDocument, aus dem die Daten gelesen werden.
     */
    void deserialize(const JsonDocument& doc);

    /**
     * @property _config
     * @brief Die Instanz der ConfigData-Struktur, die die Konfigurationswerte im RAM hält.
     */
    settings _config;

    /**
     * @property _filename
     * @brief Der Name der Konfigurationsdatei.
     */
    const char* _filename;
};
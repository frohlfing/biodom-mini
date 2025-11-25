#include "SettingsManager.h"
#include <LittleFS.h>

SettingsManager::SettingsManager(const char* filename)
    : _filename(filename) {}

bool SettingsManager::begin() {
    // Versuche, die Konfigurationsdatei zum Lesen zu öffnen.
    File configFile = LittleFS.open(_filename, "r");

    // Wenn die Datei nicht existiert, erstelle eine Standardkonfiguration.
    if (!configFile) {
        Serial.printf("Hinweis: Konfigurationsdatei '%s' nicht gefunden, erstelle Standard...\n", _filename);
        _config = settings();
        return save();
    }

    // Wenn die Datei existiert, parse sie.
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    // Wenn beim Parsen ein Fehler auftritt, lade die sicheren Standardwerte.
    if (error) {
        Serial.printf("FEHLER: Konnte '%s' nicht parsen, lade Standardwerte. Fehler: %s\n", _filename, error.c_str());
        _config = settings();
        return false;
    }

    // Wenn alles gut ging, fülle die Konfigurationsstruktur mit den Werten aus der Datei.
    deserialize(doc);
    return true;
}

bool SettingsManager::save() const {
    // Öffne die Konfigurationsdatei zum Schreiben.
    File configFile = LittleFS.open(_filename, "w");
    if (!configFile) {
        Serial.printf("FEHLER: Konnte Konfigurationsdatei '%s' nicht zum Schreiben öffnen!\n", _filename);
        return false;
    }

    // Erstelle ein JSON-Dokument und fülle es mit der serialize()-Methode der abgeleiteten Klasse.
    JsonDocument doc;
    serialize(doc);

    // Schreibe das JSON-Dokument in die Datei.
    if (serializeJson(doc, configFile) == 0) {
        Serial.printf("FEHLER: Konnte Konfiguration nicht in '%s' schreiben.\n", _filename);
        configFile.close();
        return false;
    }

    configFile.close();
    Serial.printf("Konfiguration erfolgreich in '%s' gespeichert.\n", _filename);
    return true;
}

const settings& SettingsManager::get() const {
    return _config;
}

settings& SettingsManager::getMutable() {
    return _config;
}

void SettingsManager::serialize(JsonDocument& doc) const {
    doc["airTempThresholdHigh"] = _config.airTempThresholdHigh;
    doc["humidityTarget"] = _config.humidityTarget;
    doc["soilTempTarget"] = _config.soilTempTarget;
    doc["soilMoistureTarget"] = _config.soilMoistureTarget;
    doc["lightOnHour"] = _config.lightOnHour;
    doc["lightOffHour"] = _config.lightOffHour;
    doc["lightLuxThresholdDark"] = _config.lightLuxThresholdDark;
    doc["lightLuxThresholdBright"] = _config.lightLuxThresholdBright;
    doc["fanCooldownDurationMs"] = _config.fanCooldownDurationMs;
    doc["wateringDurationMs"] = _config.wateringDurationMs;
}

void SettingsManager::deserialize(const JsonDocument& doc) {
    _config.airTempThresholdHigh = doc["airTempThresholdHigh"] | _config.airTempThresholdHigh;
    _config.humidityTarget = doc["humidityTarget"] | _config.humidityTarget;
    _config.soilTempTarget = doc["soilTempTarget"] | _config.soilTempTarget;
    _config.soilMoistureTarget = doc["soilMoistureTarget"] | _config.soilMoistureTarget;
    _config.lightOnHour = doc["lightOnHour"] | _config.lightOnHour;
    _config.lightOffHour = doc["lightOffHour"] | _config.lightOffHour;
    _config.lightLuxThresholdDark = doc["lightLuxThresholdDark"] | _config.lightLuxThresholdDark;
    _config.lightLuxThresholdBright = doc["lightLuxThresholdBright"] | _config.lightLuxThresholdBright;
    _config.fanCooldownDurationMs = doc["fanCooldownDurationMs"] | _config.fanCooldownDurationMs;
    _config.wateringDurationMs = doc["wateringDurationMs"] | _config.wateringDurationMs;
}
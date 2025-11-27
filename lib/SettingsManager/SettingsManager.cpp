#include "SettingsManager.h"
#include <LittleFS.h>

SettingsManager::SettingsManager(const char* filename)
    : _filename(filename) {}

bool SettingsManager::begin() {
    // Versuche, die JSON-Datei zum Lesen zu öffnen.
    File configFile = LittleFS.open(_filename, "r");

    // Wenn die Datei nicht existiert, erstelle eine Standardkonfiguration.
    if (!configFile) {
        Serial.printf("Hinweis: JSON-Datei '%s' nicht gefunden, erstelle Standard...\n", _filename);
        _settings = Settings();
        return save();
    }

    // Wenn die Datei existiert, parse sie.
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    // Wenn beim Parsen ein Fehler auftritt, lade die sicheren Standardwerte.
    if (error) {
        Serial.printf("FEHLER: Konnte '%s' nicht parsen, lade Standardwerte. Fehler: %s\n", _filename, error.c_str());
        _settings = Settings();
        return false;
    }

    // Wenn alles gut ging, fülle die Konfigurationsstruktur mit den Werten aus der Datei.
    const JsonObject root = doc.as<JsonObject>(); // das JsonDocument in ein JsonObject umwandeln
    deserialize(root);
    return true;
}

bool SettingsManager::save() const {
    // Öffne die JSON-Datei zum Schreiben.
    File configFile = LittleFS.open(_filename, "w");
    if (!configFile) {
        Serial.printf("FEHLER: Konnte JSON-Datei '%s' nicht zum Schreiben öffnen!\n", _filename);
        return false;
    }

    // Erstelle ein JSON-Objekt und fülle es mit der serialize()-Methode der abgeleiteten Klasse.
    JsonDocument doc;
    JsonObject root = doc.to<JsonObject>();
    serialize(root);

    // Schreibe das JSON-Objekt in die Datei.
    if (serializeJson(doc, configFile) == 0) {
        Serial.printf("FEHLER: Konnte Konfiguration nicht in '%s' schreiben.\n", _filename);
        configFile.close();
        return false;
    }

    configFile.close();
    Serial.printf("Konfiguration erfolgreich in '%s' gespeichert.\n", _filename);
    return true;
}

const Settings& SettingsManager::get() const {
    return _settings;
}

Settings& SettingsManager::getMutable() {
    return _settings;
}

void SettingsManager::serialize(const JsonObject& doc) const {
    doc["airTempThresholdHigh"] = _settings.airTempThresholdHigh;
    doc["humidityTarget"] = _settings.humidityTarget;
    doc["soilTempTarget"] = _settings.soilTempTarget;
    doc["soilMoistureTarget"] = _settings.soilMoistureTarget;

    doc["light1OnHour"] = _settings.light1OnHour;
    doc["light1OffHour"] = _settings.light1OffHour;
    doc["light1LuxThresholdDark"] = _settings.light1LuxThresholdDark;
    doc["light1LuxThresholdBright"] = _settings.light1LuxThresholdBright;

    doc["light2OnHour"] = _settings.light2OnHour;
    doc["light2OffHour"] = _settings.light2OffHour;
    doc["light2LuxThresholdDark"] = _settings.light2LuxThresholdDark;
    doc["light2LuxThresholdBright"] = _settings.light2LuxThresholdBright;

    doc["fanCooldownDurationMs"] = _settings.fanCooldownDurationMs;
    doc["wateringDurationMs"] = _settings.wateringDurationMs;

    // Konvertiere die ControlMode-Enums in Strings für JSON
    auto modeToString = [](const ControlMode mode) {
        if (mode == MODE_ON) return "on";
        if (mode == MODE_OFF) return "off";
        return "auto";
    };
    doc["lamp1Mode"] = modeToString(_settings.lamp1Mode);
    doc["lamp2Mode"] = modeToString(_settings.lamp2Mode);
    doc["heaterMode"] = modeToString(_settings.heaterMode);
    doc["fanMode"] = modeToString(_settings.fanMode);
    doc["pumpMode"] = modeToString(_settings.pumpMode);
    doc["misterMode"] = modeToString(_settings.misterMode);
}

void SettingsManager::deserialize(const JsonObject& doc) {
    _settings.airTempThresholdHigh = doc["airTempThresholdHigh"] | _settings.airTempThresholdHigh;
    _settings.humidityTarget = doc["humidityTarget"] | _settings.humidityTarget;
    _settings.soilTempTarget = doc["soilTempTarget"] | _settings.soilTempTarget;
    _settings.soilMoistureTarget = doc["soilMoistureTarget"] | _settings.soilMoistureTarget;

    _settings.light1OnHour = doc["light1OnHour"] | _settings.light1OnHour;
    _settings.light1OffHour = doc["light1OffHour"] | _settings.light1OffHour;
    _settings.light1LuxThresholdDark = doc["light1LuxThresholdDark"] | _settings.light1LuxThresholdDark;
    _settings.light1LuxThresholdBright = doc["light1LuxThresholdBright"] | _settings.light1LuxThresholdBright;

    _settings.light2OnHour = doc["light2OnHour"] | _settings.light2OnHour;
    _settings.light2OffHour = doc["light2OffHour"] | _settings.light2OffHour;
    _settings.light2LuxThresholdDark = doc["light2LuxThresholdDark"] | _settings.light2LuxThresholdDark;
    _settings.light2LuxThresholdBright = doc["light2LuxThresholdBright"] | _settings.light2LuxThresholdBright;

    _settings.fanCooldownDurationMs = doc["fanCooldownDurationMs"] | _settings.fanCooldownDurationMs;
    _settings.wateringDurationMs = doc["wateringDurationMs"] | _settings.wateringDurationMs;

    // Konvertiere die Strings aus JSON zurück in die ControlMode-Enums
    auto stringToMode = [](const char* str, const ControlMode defaultMode) {
        if (strcmp(str, "on") == 0) return MODE_ON;
        if (strcmp(str, "off") == 0) return MODE_OFF;
        return defaultMode; // Standard ist 'auto'
    };
    _settings.lamp1Mode = stringToMode(doc["lamp1Mode"] | "auto", _settings.lamp1Mode);
    _settings.lamp2Mode = stringToMode(doc["lamp2Mode"] | "auto", _settings.lamp2Mode);
    _settings.heaterMode = stringToMode(doc["heaterMode"] | "auto", _settings.heaterMode);
    _settings.fanMode = stringToMode(doc["fanMode"] | "auto", _settings.fanMode);
    _settings.pumpMode = stringToMode(doc["pumpMode"] | "auto", _settings.pumpMode);
    _settings.misterMode = stringToMode(doc["misterMode"] | "auto", _settings.misterMode);
}
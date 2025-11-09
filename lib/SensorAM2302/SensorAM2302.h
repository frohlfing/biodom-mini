#pragma once

#include <Arduino.h>
#include <SimpleDHT.h>

/**
 * Klasse für den Raumtemperatur- und Luftfeuchtigkeitssensor AM2302 (DHT22).
 */
class SensorAM2302 {
public:
    /**
     * @brief Konstruktor mit Angabe des GPIO-Pins.
     * @param pin GPIO-Pin, an dem der Sensor angeschlossen ist.
     */
    explicit SensorAM2302(uint8_t pin);

    /**
     * @brief Initialisiert den Sensor.
     * Führt eine erste Testlesung durch, um die Verbindung zu prüfen.
     * @return true bei Erfolg, andernfalls false.
     */
    bool begin();
    
    /**
     * @brief Liest die Raumtemperatur und Luftfeuchtigkeit vom Sensor.
     * @return true bei erfolgreichem Auslesen, false bei Fehler.
     */
    bool read();

    /**
     * @brief Gibt die zuletzt gemessene Temperatur in °C zurück.
     * @return Temperatur (float), oder NAN bei Fehler.
     */
    float getTemperature() const;

    /**
     * @brief Gibt die zuletzt gemessene relative Luftfeuchtigkeit in Prozent zurück.
     * @return Luftfeuchtigkeit (float), , oder NAN bei Fehler.
     */
    float getHumidity() const;

    /**
     * @brief Gibt den letzten Fehlercode zurück.
     * @return Fehlercode gemäß SimpleDHT-Definition.
     */
    int getLastError() const;

    /**
     * @brief Gibt eine Beschreibung des letzten Fehlers zurück.
     * @return Fehlerbeschreibung (String).
     */
    const char* getErrorMessage() const;

private:
    uint8_t _pin;           // GPIO-Pin des Sensors
    SimpleDHT22 _sensor;    // SimpleDHT22-Instanz
    float _temperature;     // Letzte gemessene Temperatur
    float _humidity;        // Letzte gemessene Luftfeuchtigkeit
    int _lastError;         // Fehlercode
};

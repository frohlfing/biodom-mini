/**
 * @file AM2302.h
 * @brief Lokale Kapselung für den Luftfeuchte- und Temperatursensor AM2302 (DHT22).
 */

#pragma once
#include <Arduino.h>
#include <SimpleDHT.h>

/**
 * @brief Klasse für den Luftsensor AM2302 (DHT22).
 */
class AM2302 {
public:
    /**
     * @brief Konstruktor mit Angabe des GPIO-Pins.
     * @param pin GPIO-Pin, an dem der Sensor angeschlossen ist.
     */
    explicit AM2302(uint8_t pin);

    /**
     * @brief Liest die Raumtemperatur und Luftfeuchtigkeit vom Sensor.
     * @return true bei erfolgreichem Auslesen, false bei Fehler.
     */
    bool read();

    /**
     * @brief Gibt die zuletzt gemessene Temperatur in °C zurück.
     * @return Temperatur in Grad Celsius (float).
     */
    float getTemperature() const;

    /**
     * @brief Gibt die zuletzt gemessene relative Luftfeuchtigkeit in Prozent zurück.
     * @return Luftfeuchtigkeit (float).
     */
    float getHumidity() const;

    /**
     * @brief Gibt den letzten Fehlercode zurück.
     * @return Fehlercode gemäß SimpleDHT-Definition.
     */
    int getLastError() const;

    /**
     * @brief Gibt eine Beschreibung des letzten Fehlers zurück.
     * @return Fehlerbeschreibung als String.
     */
    const char* getErrorMessage() const;

private:
    uint8_t _pin;
    SimpleDHT22 _sensor;
    float _temperature;
    float _humidity;
    int _lastError;
};

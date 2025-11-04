/**
 * @file SensorCapacitiveSoil.h
 * @brief Lokale Kapselung für den kapazitiven Bodenfeuchtesensor V1.2 (analog).
 */

#pragma once
#include <Arduino.h>

/**
 * @brief Klasse für den kapazitiven Bodenfeuchtesensor V1.2 (analog).
 *
 * Wandelt den analogen Messwert in Prozent um (0–100 %).
 */
class SensorCapacitiveSoil {
public:
    /**
     * @brief Konstruktor mit Angabe des analogen Eingangs und optionalen Kalibrierungswerten.
     * @param analogPin GPIO-Analog-Pin, an dem der Sensor angeschlossen ist.
     * @param dryValue Rohwert für trockenen Boden (höherer Analogwert).
     * @param wetValue Rohwert für nassen Boden (niedrigerer Analogwert).
     */
    explicit SensorCapacitiveSoil(uint8_t analogPin, int dryValue = 2500, int wetValue = 1100);

    /**
     * @brief Initialisiert die Klasse (aktuell: Platzhalter, setzt Pin-Modus).
     */
    void begin();

    /**
     * @brief Liest die aktuellen Sensorwerte und speichert sie intern.
     * @return true bei erfolgreichem Auslesen.
     */
    bool read();

    /**
     * @brief Gibt den zuletzt gelesenen Rohwert zurück.
     * @return Rohwert (int).
     */
    int getRaw() const;

    /**
     * @brief Gibt die zuletzt berechnete Bodenfeuchte in Prozent zurück.
     * @return Feuchtigkeit in Prozent (0–100).
     */
    int getPercent() const;

    /**
     * @brief Setzt Kalibrierungswerte zur Laufzeit.
     * @param dryValue Rohwert für trockenen Boden.
     * @param wetValue Rohwert für nassen Boden.
     */
    void setCalibration(int dryValue, int wetValue);

private:
    uint8_t _pin;   // Analogpin
    int _dry;       // Rohwert dry (trocken)
    int _wet;       // Rohwert wet (nass)
    int _lastRaw;   // letzter Rohwert
    int _lastPct;   // letzter Prozentwert
};

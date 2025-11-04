/**
 * @file SensorXKCY25NPN.h
 * @brief Lokale Kapselung für den berührungslosen Füllstandssensor XKC-Y25-NPN (NPN, digital).
 */

#pragma once
#include <Arduino.h>

/**
 * @brief Klasse für den berührungslosen Füllstandssensor XKC-Y25-NPN.
 *
 * Der Sensor liefert einen digitalen NPN-Ausgang (LOW bei Wassererkennung).
 * Üblicherweise wird am Ausgang ein Pull‑Up auf +3.3V benötigt (10 kΩ).
 */
class SensorXKCY25NPN {
public:
    /**
     * @brief Konstruktor.
     * @param pin GPIO-Pin, an den der digitale Ausgang (OUT) des Sensors angeschlossen ist.
     * @param useInternalPullup Wenn true, wird pinMode(INPUT_PULLUP) genutzt (falls geeignet).
     */
    explicit SensorXKCY25NPN(uint8_t pin, bool useInternalPullup = false);

    /**
     * @brief Initialisiert den Sensor (Pinmodus).
     */
    void begin();

    /**
     * @brief Liest den aktuellen Zustand des Sensors.
     * @return true, wenn Wasser erkannt wurde; false sonst.
     */
    bool read();

    /**
     * @brief Gibt den zuletzt gelesenen Rohzustand zurück.
     * @return true = Wasser erkannt, false = kein Wasser.
     */
    bool isWaterDetected() const;

    /**
     * @brief Gibt eine kurze Beschreibung der empfohlenen Verdrahtung zurück.
     * @return const char* mit Hinweis auf Pull‑Up und Versorgungsspannung.
     */
    const char* wiringNotes() const;

private:
    uint8_t _pin;
    bool _useInternalPullup;
    bool _lastState;
};

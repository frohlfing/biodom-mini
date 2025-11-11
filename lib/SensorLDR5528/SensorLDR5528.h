#pragma once
#include <Arduino.h>

/**
 * Klasse für den Fotowiderstand LDR5528
 * 
 * Liefert Roh-ADC-Wert und eine approximate "Lux"-Schätzung (relativ).
 *
 * Annahmen zur Schaltung (Pull-Down-Konfiguration):
 * - Der LDR ist Teil eines Spannungsteilers: VCC -- [LDR] -- AIN -- [R_fixed] -- GND
 * - R_fixed ist der feste Widerstand in Ohm.
 */
class SensorLDR5528 {
public:
    /**
     * Konstruktor.
     * @param analogPin  ADC-Pin, an dem gemessen wird (AIN).
     * @param fixedResistorOhm Wert des festen Widerstands in Ohm (Standard 10kΩ).
     * @param adcMax Maximaler ADC-Wert (z. B. 1023 für AVR, 4095 für ESP32)
     */
    explicit SensorLDR5528(uint8_t analogPin, float pullupOhm = 10000.0f, uint16_t adcMax = 4095);

    /**
     * @brief Initialisiert den Sensor.
     * Führt eine erste Testmessung durch, um die Verbindung zu prüfen.
     * @return true bei Erfolg, andernfalls false.
     */
    bool begin();

    /**
     * @brief Liest den ADC und berechnet Widerstand und geschätzte Lux.
     * @return true bei erfolgreichem Auslesen, false bei Fehler.
     */
    bool read();

    /** Roh-ADC-Wert (0..adcMax). */
    uint16_t getRaw() const;

    /** Berechneter LDR-Widerstand in Ohm. */
    float getResistance() const;

    /** Geschätzter Lux-Wert (approximativ). Gibt NAN bei unbekannt zurück. */
    float getLux() const;

    /** Setzt maximalen ADC-Wert (z. B. 1023 oder 4095). */
    void setAdcMax(uint16_t adcMax);

    /** Setzt den Wert des festen Widerstands Ohm. */
    void setFixedResistorOhm(float fixedResistorOhm);

    /**
     * @brief Gibt den letzten Fehlercode zurück.
     * @return Fehlercode (0=OK, 1=Wert unplausibel/Kurzschluss)
     */
    int getLastError() const;

    /**
     * @brief Gibt eine Beschreibung des letzten Fehlers zurück.
     * @return Fehlerbeschreibung.
     */
    const char* getErrorMessage() const;

private:
    uint8_t _pin;
    float _fixedResistorOhm;
    uint16_t _adcMax;
    uint16_t _raw;
    float _resistance;  // Ohm
    float _lux;         // approximativ
    int _lastError;     // Speichert den Fehlercode der letzten Operation (0 = OK).

    /** Hilfsfunktion: ADC -> Spannung (0..Vcc) */
    float adcToVoltage(uint16_t adc) const;

    /** Hilfsfunktion: Widerstand -> approximativer Lux (empirische Formel) */
    float resistanceToLux(float r) const;
};

/**
 * @file SensorBH1750.h
 * @brief Lokale Kapselung für das Lichtsensormodul GY-302 (BH1750, I2C).
 */

#pragma once
#include <Arduino.h>
#include <BH1750.h>

/**
 * @brief Klasse für das Lichtsensormodul GY-302.
 */
class SensorBH1750 {
public:
    /**
     * @brief Fehlercodes der Klasse.
     */
    enum Error {
        ErrNone = 0,
        ErrNotInitialized = 1,
        ErrReadFailed = 2
    };

    /**
     * @brief Konstruktor.
     * @param address I2C-Adresse des Sensors (Standard: 0x23).
     * @param wire Pointer auf TwoWire-Instanz (Default: &Wire).
     */
    explicit SensorBH1750(uint8_t address = 0x23, TwoWire* wire = &Wire);

    /**
     * @brief Initialisiert das I2C-Interface und den BH1750-Treiber.
     * @return true bei erfolgreicher Initialisierung, false bei Fehler.
     */
    bool begin();

    /**
     * @brief Führt eine Messung durch und speichert den aktuellen Lux-Wert intern.
     * @return true bei Erfolg, false bei Fehler (Fehlercode über getLastError()).
     */
    bool read();

    /**
     * @brief Liefert den zuletzt gemessenen Lux-Wert.
     * @return Lux als float; bei Fehler wird NAN zurückgegeben.
     */
    float getLux() const;

    /**
     * @brief Liefert den zuletzt gesetzten Fehlercode.
     * @return Error-Code.
     */
    Error getLastError() const;

    /**
     * @brief Liefert eine kurze, lesbare Fehlermeldung zum letzten Fehler.
     * @return C-String mit Beschreibung.
     */
    const char* getErrorMessage() const;

    /**
     * @brief Setzt den Messmodus des internen BH1750-Treibers.
     * @param mode BH1750-Modus aus der BH1750-Bibliothek (z. B. BH1750::CONT_HIGH_RES_MODE).
     */
    void setMode(BH1750::Mode mode);

private:
    TwoWire* _wire;
    uint8_t _addr;
    BH1750 _drv;
    float _lastLux;
    Error _lastError;
    BH1750::Mode _mode;
};

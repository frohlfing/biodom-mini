#include "SensorBH1750.h"

SensorBH1750::SensorBH1750(uint8_t address, TwoWire* wire)
    : _wire(wire),
      _addr(address),
      _drv(BH1750(address)),
      _lastLux(NAN),
      _lastError(ErrNotInitialized),
      _mode(BH1750::CONTINUOUS_HIGH_RES_MODE) {}

bool SensorBH1750::begin() {
    if (!_wire) {
        _lastError = ErrNotInitialized;
        return false;
    }

    _wire->begin();
    // BH1750 library erwartet, dass Wire bereits initialisiert ist.
    if (!_drv.begin(_mode)) {
        _lastError = ErrNotInitialized;
        return false;
    }

    _lastError = ErrNone;
    return true;
}

bool SensorBH1750::read() {
    // Die claws/BH1750 Bibliothek liefert negative Werte bei Fehler (konvention der Lib).
    float lux = _drv.readLightLevel();
    if (lux < 0.0f) {
        _lastLux = NAN;
        _lastError = ErrReadFailed;
        return false;
    }

    _lastLux = lux;
    _lastError = ErrNone;
    return true;
}

float SensorBH1750::getLux() const {
    return _lastLux;
}

SensorBH1750::Error SensorBH1750::getLastError() const {
    return _lastError;
}

const char* SensorBH1750::getErrorMessage() const {
    switch (_lastError) {
        case ErrNone: return "Erfolgreich";
        case ErrNotInitialized: return "Nicht initialisiert oder I2C nicht verfügbar";
        case ErrReadFailed: return "Messung fehlgeschlagen";
        default: return "Unbekannter Fehler";
    }
}

void SensorBH1750::setMode(BH1750::Mode mode) {
    _mode = mode;
    // Reconfigure driver if already initialized
    if (_lastError == ErrNone) {
        // begin(mode) in claws/BH1750 setzt den Modus neu
        _drv.begin(_mode);
    }
}

#include "AM2302.h"

AM2302::AM2302(uint8_t pin)
    : _pin(pin), _sensor(pin), _temperature(NAN), _humidity(NAN), _lastError(SimpleDHTErrSuccess) {}

bool AM2302::read() {
    _lastError = _sensor.read2(&_temperature, &_humidity, nullptr);
    if (_lastError != SimpleDHTErrSuccess) {
        _temperature = NAN;
        _humidity = NAN;
        return false;
    }
    return true;
}

float AM2302::getTemperature() const {
    return _temperature;
}

float AM2302::getHumidity() const {
    return _humidity;
}

int AM2302::getLastError() const {
    return _lastError;
}

const char* AM2302::getErrorMessage() const {
    switch (_lastError) {
        case SimpleDHTErrSuccess: return "Erfolgreich";
        case SimpleDHTErrStartLow: return "Fehler beim Warten auf Startsignal (LOW)";
        case SimpleDHTErrStartHigh: return "Fehler beim Warten auf Startsignal (HIGH)";
        case SimpleDHTErrDataLow: return "Fehler beim Warten auf das Datenstartsignal (LOW)";
        case SimpleDHTErrDataRead: return "Fehler beim Lesen der Datenbits";
        case SimpleDHTErrDataEOF: return "Fehler: Datenübertragung unvollständig";
        case SimpleDHTErrDataChecksum: return "Fehlerhafte Prüfsumme";
        case SimpleDHTErrZeroSamples: return "Fehler: Messwerte sind Null";
        case SimpleDHTErrNoPin: return "Fehler: Pin nicht initialisiert";
        case SimpleDHTErrPinMode: return "Fehler: Ungültiger Pin-Modus";
        default: return "Unbekannter Fehler";
    }
}

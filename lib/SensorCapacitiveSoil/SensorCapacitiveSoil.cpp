#include "SensorCapacitiveSoil.h"

SensorCapacitiveSoil::SensorCapacitiveSoil(uint8_t analogPin, int dryValue, int wetValue)
    : _pin(analogPin), _dry(dryValue), _wet(wetValue), _lastRaw(0), _lastPct(0) {}

void SensorCapacitiveSoil::begin() {
    // Analogpins brauchen unter ESP32 keinen speziellen pinMode, aber setzen wir ihn zur Klarheit:
    pinMode(_pin, INPUT);
}

bool SensorCapacitiveSoil::read() {
    // analogRead Rückgabewerte können je nach Plattform variieren (0..4095 auf ESP32)
    int val = analogRead(_pin);
    _lastRaw = val;

    // Mappe Rohwert auf Prozent. Annahme: dry > wet. Wenn invertiert, wird Ergebnis begrenzt.
    int pct = map(val, _dry, _wet, 0, 100);
    // Falls dry < wet (ungewöhnlich), korrigiere die Zuordnung
    if (_dry < _wet) {
        pct = map(val, _wet, _dry, 0, 100);
    }

    _lastPct = constrain(pct, 0, 100);
    return true;
}

int SensorCapacitiveSoil::getRaw() const {
    return _lastRaw;
}

int SensorCapacitiveSoil::getPercent() const {
    return _lastPct;
}

void SensorCapacitiveSoil::setCalibration(int dryValue, int wetValue) {
    _dry = dryValue;
    _wet = wetValue;
}

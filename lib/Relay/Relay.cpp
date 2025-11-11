#include "Relay.h"

Relay::Relay(uint8_t pin, bool activeHigh, bool safeState)
    : _pin(pin), _activeHigh(activeHigh), _state(false), _hasPulse(false), _pulseStart(0), _pulseDur(0), _safeState(safeState) {}

void Relay::begin() {
    pinMode(_pin, OUTPUT);
    // Initialzustand: safeState (meistens AUS)
    _state = _safeState;
    writePin(_state);
    _hasPulse = false;
    _pulseStart = 0;
    _pulseDur = 0;
}

void Relay::on() {
    _hasPulse = false;
    _state = true;
    writePin(_state);
}

void Relay::off() {
    _hasPulse = false;
    _state = false;
    writePin(_state);
}

void Relay::toggle() {
    _hasPulse = false;
    _state = !_state;
    writePin(_state);
}

void Relay::pulse(unsigned long durationMs) {
    if (durationMs == 0) return;
    // Falls bereits ein Pulse läuft, verlängern wir ihn auf max(currentEnd, newEnd)
    unsigned long now = millis();
    if (_hasPulse) {
        unsigned long currentEnd = _pulseStart + _pulseDur;
        unsigned long newEnd = now + durationMs;
        if (newEnd > currentEnd) {
            _pulseDur = (newEnd - _pulseStart);
        }
        // falls newEnd <= currentEnd: nichts ändern
    } else {
        // Starte neuen Pulse: setze Relais EIN (temporär) und merke bisherigen Zustand
        _hasPulse = true;
        _pulseStart = now;
        _pulseDur = durationMs;
        _state = true;
        writePin(true);
    }
}

void Relay::update() {
    if (!_hasPulse) return;
    unsigned long now = millis();
    // Achte auf Overflow bei millis(): (now - start) ist sicher
    if (now - _pulseStart >= _pulseDur) {
        // Pulse beendet: setze Relais zurück in safeState (oder false)
        _hasPulse = false;
        _pulseDur = 0;
        _pulseStart = 0;
        // Rückfall auf safeState (oder false): hier entscheiden wir, dass Pulse temporär einschalten,
        // und danach ins safe(off)-Zustand zurückkehren. Falls anderes gewünscht, kann API erweitert werden.
        _state = _safeState;
        writePin(_state);
    }
}

bool Relay::isOn() const {
    return _state;
}

uint8_t Relay::pin() const {
     return _pin;
}

void Relay::writePin(bool logicalOn) {
    // Mappe logisches "ON" auf physikalischen Pegel je nach activeHigh
    if (_activeHigh) {
        digitalWrite(_pin, logicalOn ? HIGH : LOW);
    } else {
        // Invertierte Logik (häufig bei Relais-Boards mit opto): LOW = Relais EIN
        digitalWrite(_pin, logicalOn ? LOW : HIGH);
    }
}

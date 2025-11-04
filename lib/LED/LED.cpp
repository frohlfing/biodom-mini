#include "LED.h"

LED::LED(uint8_t pin, bool activeHigh)
  : _pin(pin),
    _activeHigh(activeHigh),
    _state(false),
    _onMs(0),
    _offMs(0),
    _lastToggle(0),
    _blinking(false) {}

void LED::begin() {
  pinMode(_pin, OUTPUT);
  off();
}

void LED::on() {
  _state = true;
  _blinking = false;
  digitalWrite(_pin, _activeHigh ? HIGH : LOW);
}

void LED::off() {
  _state = false;
  _blinking = false;
  digitalWrite(_pin, _activeHigh ? LOW : HIGH);
}

void LED::toggle() {
  if (_state) off();
  else on();
}

void LED::blink(unsigned long onMs, unsigned long offMs) {
  _onMs = onMs;
  _offMs = offMs;
  if (_onMs == 0 && _offMs == 0) {
    _blinking = false;
    return;
  }
  _blinking = true;
  // Start mit Einschalten
  _state = true;
  digitalWrite(_pin, _activeHigh ? HIGH : LOW);
  _lastToggle = millis();
}

void LED::update() {
  if (!_blinking) return;

  unsigned long now = millis();
  if (_state) {
    // aktuell ein, prüfen ob onMs abgelaufen
    if (_onMs > 0 && (now - _lastToggle >= _onMs)) {
      // schalte aus
      _state = false;
      digitalWrite(_pin, _activeHigh ? LOW : HIGH);
      _lastToggle = now;
    }
  } else {
    // aktuell aus, prüfen ob offMs abgelaufen
    if (_offMs > 0 && (now - _lastToggle >= _offMs)) {
      // schalte ein
      _state = true;
      digitalWrite(_pin, _activeHigh ? HIGH : LOW);
      _lastToggle = now;
    }
  }
}

bool LED::isOn() const {
  return _state;
}

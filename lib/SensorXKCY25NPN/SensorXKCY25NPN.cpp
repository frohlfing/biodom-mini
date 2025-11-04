#include "SensorXKCY25NPN.h"

SensorXKCY25NPN::SensorXKCY25NPN(uint8_t pin, bool useInternalPullup)
    : _pin(pin), _useInternalPullup(useInternalPullup), _lastState(false) {}

void SensorXKCY25NPN::begin() {
    if (_useInternalPullup) {
        pinMode(_pin, INPUT_PULLUP);
    } else {
        pinMode(_pin, INPUT);
    }
}

bool SensorXKCY25NPN::read() {
    // Sensor: OUT pulled LOW when water is present (NPN sink).
    // Mit externem Pull-Up zieht OUT bei Trockenheit auf HIGH.
    int v = digitalRead(_pin);
    // Wenn OUT == LOW -> Wasser erkannt
    _lastState = (v == LOW);
    return _lastState;
}

bool SensorXKCY25NPN::isWaterDetected() const {
    return _lastState;
}

const char* SensorXKCY25NPN::wiringNotes() const {
    return "VCC: 5-12V; OUT: open-collector NPN -> use 10k pull-up to 3.3V; GND common";
}

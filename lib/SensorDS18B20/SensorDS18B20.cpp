#include "SensorDS18B20.h"

SensorDS18B20::SensorDS18B20(uint8_t pin)
    : _oneWire(pin), _sensor(&_oneWire), _temperature(NAN), _lastError(0) {
    _sensor.begin();
    if (!findSensor()) {
        _lastError = 1; // Kein Sensor gefunden
    }
}

bool SensorDS18B20::findSensor() {
    if (_sensor.getDeviceCount() == 0) {
        return false;
    }
    if (!_sensor.getAddress(_addr, 0)) {
        return false;
    }
    return _sensor.validAddress(_addr);
}

bool SensorDS18B20::read() {
    if (_lastError == 1) return false;

    _sensor.requestTemperatures();
    float temp = _sensor.getTempC(_addr);

    if (temp == DEVICE_DISCONNECTED_C) {
        _temperature = NAN;
        _lastError = 2; // Sensor getrennt oder nicht lesbar
        return false;
    }

    _temperature = temp;
    _lastError = 0;
    return true;
}

float SensorDS18B20::getTemperature() const {
    return _temperature;
}

int SensorDS18B20::getLastError() const {
    return _lastError;
}

const char* SensorDS18B20::getErrorMessage() const {
    switch (_lastError) {
        case 0: return "OK";
        case 1: return "Sensor nicht gefunden";
        case 2: return "Sensor getrennt oder nicht lesbar";
        default: return "Unbekannter Fehler";
    }
}

#include "SensorAM2302.h"

SensorAM2302::SensorAM2302(uint8_t pin)
    : _pin(pin), _sensor(pin), _temperature(NAN), _humidity(NAN), _lastError(SimpleDHTErrSuccess) {}

bool SensorAM2302::begin() {
    // Für den AM2302 besteht die "Initialisierung" aus einem ersten
    // erfolgreichen Leseversuch, um die Verbindung zu verifizieren.
    return read();
}

bool SensorAM2302::read() {
    // DHT-Sensoren können aufgrund von Timing-Problemen (Interrupts) fehlschlagen.
    // Ein "Retry"-Mechanismus ist die Standardlösung, um die Zuverlässigkeit zu erhöhen.
    int retries = 3; // Wir versuchen es bis zu 3 Mal.
    for (int i = 0; i < retries; i++) {
        _lastError = _sensor.read2(&_temperature, &_humidity, nullptr);
        
        // Wenn die Lesung erfolgreich war, beenden wir die Schleife sofort.
        if (_lastError == SimpleDHTErrSuccess) {
            return true;
        }
        
        // Eine kurze Pause vor dem nächsten Versuch, damit der Sensor sich "erholen" kann.
        delay(50); 
    }

    // Wenn alle Versuche fehlgeschlagen sind, geben wir false zurück.
    // _lastError enthält den Fehler des letzten Versuchs.
    _temperature = NAN;
    _humidity = NAN;
    return false;
    
    // // Alte Variante (nur ein Leseversuch):
    // _lastError = _sensor.read2(&_temperature, &_humidity, nullptr);
    // if (_lastError != SimpleDHTErrSuccess) {
    //     _temperature = NAN;
    //     _humidity = NAN;
    //     return false;
    // }
    // return true;
}

float SensorAM2302::getTemperature() const {
    return _temperature;
}

float SensorAM2302::getHumidity() const {
    return _humidity;
}

int SensorAM2302::getLastError() const {
    return _lastError;
}

const char* SensorAM2302::getErrorMessage() const {
    switch (_lastError) {
        case SimpleDHTErrSuccess: return "Erfolgreich";
        case SimpleDHTErrStartLow: return "Timeout: Start Low";
        case SimpleDHTErrStartHigh: return "Timeout: Start High";
        case SimpleDHTErrDataLow: return "Timeout: Daten Low";
        case SimpleDHTErrDataRead: return "Daten-Lesefehler";
        case SimpleDHTErrDataEOF: return "Daten unvollstaendig";
        case SimpleDHTErrDataChecksum: return "Pruefsummenfehler";
        case SimpleDHTErrZeroSamples: return "Messwerte sind Null";
        case SimpleDHTErrNoPin: return "Pin nicht gesetzt";
        case SimpleDHTErrPinMode: return "Falscher Pin-Modus";
        default:
            return "Sensor nicht bereit";
            //static char buffer[50]; // 'static' sorgt dafür, dass der Speicher nicht auf dem Stack verloren geht, wenn die Funktion verlassen wird.
            //snprintf(buffer, sizeof(buffer), "Fehlercode %d", _lastError);
            //return buffer;
    }
}

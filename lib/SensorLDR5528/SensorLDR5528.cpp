#include "SensorLDR5528.h"
#include <math.h>

SensorLDR5528::SensorLDR5528(uint8_t analogPin, float pullupOhm, uint16_t adcMax)
  : _pin(analogPin),
    _pullupOhm(pullupOhm),
    _adcMax(adcMax),
    _raw(0),
    _resistance(NAN),
    _lux(NAN) {}

bool SensorLDR5528::begin() {
  // Für analoge Eingänge ist normalerweise keine weitere Init nötig.
  // Bei Plattformen, die ADC-Referenz setzen müssen, kann hier ergänzt werden.
  pinMode(_pin, INPUT);
  return true;
}

bool SensorLDR5528::read() {
  // Lese Rohwert
  uint32_t sum = 0;
  const uint8_t samples = 5;
  for (uint8_t i = 0; i < samples; ++i) {
    sum += analogRead(_pin);
    delay(5);
  }
  _raw = static_cast<uint16_t>(sum / samples);

  // Schütze gegen Division durch Null
  if (_raw >= _adcMax) {
    // sehr hell oder Kurzschluss zu Vcc; Widerstand gegen 0
    _resistance = 0.0f;
    _lux = NAN;
    return true;
  }

  // Spannung am Messpunkt (Vout)
  float vout = adcToVoltage(_raw);

  // Vermeide fehlerhafte Werte: wenn vout == 0 => LDR unendlich (offen)
  if (vout <= 0.0f) {
    _resistance = INFINITY;
    _lux = NAN;
    return true;
  }

  // Annahme: Spannungsteiler: Vcc -> R_pullup -> Vout -> LDR -> GND
  // R_LDR = R_pullup * (Vcc / Vout - 1)
  // Wir nehmen Vcc = 3.3V oder 5V je nach Plattform; implizit via Referenzspannung ADC.
#if defined(ARDUINO_ARCH_ESP32)
  const float vcc = 3.3f; // typischer Wert; wenn anders, anpassen
#else
  const float vcc = 5.0f;
#endif

  // Schütze vor unrealistischen Divisionen
  if (vout >= vcc) {
    _resistance = 0.0f;
    _lux = NAN;
    return true;
  }

  _resistance = _pullupOhm * (vcc / vout - 1.0f);

  // Approximative Umrechnung von Widerstand auf Lux.
  // Diese Formel ist empirisch; kalibrieren für exakte Werte.
  _lux = resistanceToLux(_resistance);

  return true;
}

uint16_t SensorLDR5528::getRaw() const {
  return _raw;
}

float SensorLDR5528::getResistance() const {
  return _resistance;
}

float SensorLDR5528::getLux() const {
  return _lux;
}

void SensorLDR5528::setAdcMax(uint16_t adcMax) {
  _adcMax = adcMax;
}

void SensorLDR5528::setPullupOhm(float pullupOhm) {
  _pullupOhm = pullupOhm;
}

float SensorLDR5528::adcToVoltage(uint16_t adc) const {
  // adc / adcMax * Vcc
#if defined(ARDUINO_ARCH_ESP32)
  const float vcc = 3.3f;
#else
  const float vcc = 5.0f;
#endif
  return (static_cast<float>(adc) / static_cast<float>(_adcMax)) * vcc;
}

float SensorLDR5528::resistanceToLux(float r) const {
  // Vereinfachte empirische Formel:
  // Für typische LDRs gilt näherungsweise R ~ a * lux^b
  // -> lux ~ (R / a)^(1/b)
  // Werte a,b grob gewählt für allgemeine LDRs; kalibrieren für Genauigkeit.
  if (!isfinite(r) || r <= 0.0f) return NAN;

  const float a = 50000.0f; // Justagewert
  const float b = -0.8f;    // Justagewert

  // lux = (r / a)^(1/b)
  float ratio = r / a;
  if (ratio <= 0.0f) return NAN;
  float lux = pow(ratio, 1.0f / b);
  return lux;
}

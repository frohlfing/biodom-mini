#pragma once
#include <Arduino.h>

/**
 * SensorLDR5528
 *
 * Einfacher Treiber für einen Fotowiderstand (LDR5528) im Spannungsteiler.
 * Liefert Roh-ADC-Wert und eine approximate "Lux"-Schätzung (relativ).
 *
 * Annahmen:
 * - Der LDR ist Teil eines Spannungsteilers: Vcc -- R_pullup -- AIN -- LDR -- GND
 * - pullupOhm ist der feste Widerstand in Ohm.
 *
 * Hinweise:
 * - Für exakte Lux-Werte ist eine Kalibrierung gegen ein lux-messgerät nötig.
 */
class SensorLDR5528 {
public:
  /**
   * Konstruktor.
   * @param analogPin Arduino-ADC-Pin (z. B. A0 oder GPIO-Pin mit ADC auf ESP32)
   * @param pullupOhm Werte des Pullup-Widerstands in Ohm (Standard 10kΩ)
   * @param adcMax Maximaler ADC-Wert (z. B. 1023 für 10-bit AVR, 4095 für 12-bit ESP32)
   */
  explicit SensorLDR5528(uint8_t analogPin, float pullupOhm = 10000.0f, uint16_t adcMax = 1023);

  /** Initialisiert intern nichts Besonderes — nur Platzhalter für API-Konsistenz. */
  bool begin();

  /** Liest den ADC und berechnet Rohwert, Widerstand und geschätzte Lux-Angabe.
   *  Gibt true zurück, wenn lesen erfolgreich war. */
  bool read();

  /** Roh-ADC-Wert (0..adcMax). */
  uint16_t getRaw() const;

  /** Berechneter LDR-Widerstand in Ohm. */
  float getResistance() const;

  /** Geschätzter Lux-Wert (approximativ). Gibt NAN bei unbekannt zurück. */
  float getLux() const;

  /** Setzt maximalen ADC-Wert (z. B. 1023 oder 4095). */
  void setAdcMax(uint16_t adcMax);

  /** Setzt Pullup-Widerstand in Ohm (falls du andere Hardware verwendest). */
  void setPullupOhm(float pullupOhm);

private:
  uint8_t _pin;
  float _pullupOhm;
  uint16_t _adcMax;

  uint16_t _raw;
  float _resistance; // Ohm
  float _lux;        // approximativ

  /** Hilfsfunktion: ADC -> Spannung (0..Vcc) */
  float adcToVoltage(uint16_t adc) const;

  /** Hilfsfunktion: Widerstand -> approximativer Lux (empirische Formel) */
  float resistanceToLux(float r) const;
};

#pragma once
#include <Arduino.h>

/**
 * LED
 *
 * Einfacher Treiber für eine einzelne LED (oder Optokoppler-Eingang, Status-LED, ...)
 * Unterstützt aktive High/Low Logik, nicht-blockierendes Blinken und Toggle.
 */
class LED {
public:
  /**
   * Konstruktor.
   * @param pin GPIO-Pin, an dem die LED angeschlossen ist.
   * @param activeHigh true, wenn HIGH die LED einschaltet (Standard: true).
   */
  explicit LED(uint8_t pin, bool activeHigh = true);

  /** Initialisiert den Pin (PinMode). */
  void begin();

  /** Schaltet die LED dauerhaft ein. */
  void on();

  /** Schaltet die LED dauerhaft aus. */
  void off();

  /** Schaltet die LED um (ein -> aus, aus -> ein). */
  void toggle();

  /**
   * Startet ein nicht-blockierendes Blinken.
   * @param onMs  Einschaltzeit in Millisekunden.
   * @param offMs Ausschaltzeit in Millisekunden.
   * Wenn onMs==0 und offMs==0 wird Blinken deaktiviert.
   */
  void blink(unsigned long onMs, unsigned long offMs);

  /**
   * Muss regelmäßig in loop() aufgerufen werden, um Blink-Transitions auszuführen.
   * Die Methode ist schnell und nicht-blockierend.
   */
  void update();

  /** Liefert den aktuellen logischen Zustand (true == LED leuchtet). */
  bool isOn() const;

private:
  uint8_t _pin;
  bool _activeHigh;
  bool _state;               // aktueller logischer Zustand (true=>LED leuchtet)
  unsigned long _onMs;       // Blink: Einschaltzeit
  unsigned long _offMs;      // Blink: Ausschaltzeit
  unsigned long _lastToggle; // Timestamp der letzten Umschaltung
  bool _blinking;
};

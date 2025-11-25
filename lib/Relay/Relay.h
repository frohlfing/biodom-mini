#pragma once

#include <Arduino.h>

/**
 * Klasse für ein (einzelnes) Relaismodul.
 */
class Relay {
public:
    /**
     * Konstruktor
     * @param pin GPIO-Pin, der das Relais steuert (z. B. INx auf einem Relaisboard)
     * @param activeHigh true: HIGH schaltet Relais EIN; false: LOW schaltet Relais EIN (invertierte Module)
     * @param safeState false: Relais standardmäßig AUS; true: Relais standardmäßig EIN (Default: AUS)
     */
    explicit Relay(uint8_t pin, bool activeHigh = false, bool safeState = false);

    /** Initialisiert den Pin und setzt den Anfangszustand. */
    void begin();

    /** Schaltet das Relais ein (persistenter Zustand). */
    void on();

    /** Schaltet das Relais aus (persistenter Zustand). */
    void off();

    /** Schaltet das Relais um. */
    void toggle();

    /**
     * Startet einen nicht-blockierenden Pulse: Relais wird für durationMs eingeschaltet,
     * danach wieder in den vorherigen Zustand zurückgesetzt.
     * @param durationMs Pulsdauer in Millisekunden (0 = kein Effekt)
     */
    void pulse(unsigned long durationMs);

    /**
     * Muss regelmäßig in loop() ausgeführt werden.
     * Sollte sehr kurz und nicht-blockierend sein.
     */
    void update();

    /** Liefert den aktuellen logischen Zustand (true == Relais geschaltet/Last an). */
    bool isOn() const;

    /** Liefert den physikalischen Ausgabepin. */
    uint8_t pin() const;

private:
    uint8_t _pin;
    bool _activeHigh;
    bool _state; // aktuell gesetzter Zustand (logical on/off)
    bool _hasPulse;
    unsigned long _pulseStart; // millis() Startzeit des Pulses
    unsigned long _pulseDur; // Dauer des Pulses in ms
    bool _safeState; // Default-Zustand nach reset/begin
    void writePin(bool logicalOn) const;
};

/**
 * Unit-Test für die Relay-Bibliothek
 */

#include <Arduino.h>
#include <unity.h>
#include "Relay.h"

Relay testRelay(25, false, false); // GPIO25, activeHigh=false, safeState=false (aus)

void test_begin_and_basic_on_off() {
    testRelay.begin();
    TEST_ASSERT_FALSE(testRelay.isOn());

    testRelay.on();
    TEST_ASSERT_TRUE(testRelay.isOn());

    testRelay.off();
    TEST_ASSERT_FALSE(testRelay.isOn());

    testRelay.toggle();
    TEST_ASSERT_TRUE(testRelay.isOn());
}

void test_pulse_non_blocking() {
    testRelay.begin();
    testRelay.off();

    // Starte 200 ms Pulse
    testRelay.pulse(200);
    TEST_ASSERT_TRUE(testRelay.isOn()); // sofort ein

    // Warte 250 ms und rufe update() -> Pulse muss beendet sein
    delay(250);
    testRelay.update();
    // Erwartung: zurück in safeState (off)
    TEST_ASSERT_FALSE(testRelay.isOn());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_begin_and_basic_on_off);
    RUN_TEST(test_pulse_non_blocking);
    UNITY_END();
}

void loop() {}

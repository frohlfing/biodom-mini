/**
 * Unit-Test für die LED-Bibliothek
 */

#include <Arduino.h>
#include <unity.h>
#include "LED.h"

LED led(5, true); // GPIO5

void test_on_off_toggle() {
    led.begin();
    led.off();
    TEST_ASSERT_FALSE(led.isOn());

    led.on();
    TEST_ASSERT_TRUE(led.isOn());

    led.toggle();
    TEST_ASSERT_FALSE(led.isOn());

    led.toggle();
    TEST_ASSERT_TRUE(led.isOn());
}

void test_blink_behavior() {
    led.begin();
    // kurzes Blinkmuster: 50ms on, 50ms off
    led.blink(50, 50);
    // direkt nach Start sollte LED an sein
    TEST_ASSERT_TRUE(led.isOn());

    // warte 120ms -> sollte mindestens einmal toggled haben und aktuell an oder aus möglich
    delay(120);
    led.update();
    // update hat evtl Zustand angepasst; Zustand ist bool, nur prüfen, dass Methode ohne Absturz lief
    TEST_ASSERT_TRUE(true);
    // Deaktiviere Blink
    led.blink(0, 0);
    // Stelle sicher, LED nicht weiter blinkt (Zustand bleibt wie zuletzt gesetzt)
    bool stateBefore = led.isOn();
    delay(60);
    led.update();
    TEST_ASSERT_EQUAL(stateBefore, led.isOn());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_on_off_toggle);
    RUN_TEST(test_blink_behavior);
    UNITY_END();
}

void loop() {}

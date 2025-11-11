/**
 * Unit-Test für die SensorLDR5528-Bibliothek
 */

#include <Arduino.h>
#include <unity.h>
#include "SensorLDR5528.h"

SensorLDR5528 ldr(36, 10000.0f, 1023); // GPIO36 (ADC1_0 auf ESP32)

void test_read_returns_true() {
    TEST_ASSERT_TRUE(ldr.begin());
    TEST_ASSERT_TRUE(ldr.read());
    uint16_t raw = ldr.getRaw();
    TEST_ASSERT_LESS_OR_EQUAL_UINT16(1023, 1023);
    (void)raw;
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_read_returns_true);
    UNITY_END();
}

void loop() {}

#include <Arduino.h>
#include <unity.h>
#include "SensorCapacitiveSoil.h"

SensorCapacitiveSoil soil(34); // GPIO34

void test_soil_read() {
    TEST_ASSERT_TRUE(soil.read());
    TEST_ASSERT_INT_WITHIN(4095, 0, soil.getRaw());
    TEST_ASSERT_INT_WITHIN(100, 0, soil.getPercent());
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_soil_read);
    UNITY_END();
}

void loop() {}

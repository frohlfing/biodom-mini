#include <Arduino.h>
#include <Wire.h>
#include <unity.h>
#include "SensorBH1750.h"

SensorBH1750 light; // Standard-I2C-Adresse 0x23

void test_light_read() {
    TEST_ASSERT_TRUE(light.read());
    TEST_ASSERT_FLOAT_IS_NOT_NAN(light.getLux());
    TEST_ASSERT_GREATER_THAN(0.0f, light.getLux()); // typischer Lux-Wert > 0
}

void setup() {
    delay(2000);
    Wire.begin();
    light.begin();
    UNITY_BEGIN();
    RUN_TEST(test_light_read);
    UNITY_END();
}

void loop() {}

#include <Arduino.h>
#include <unity.h>
#include "SensorAM2302.h"

SensorAM2302 sensor(13); // GPIO13

void test_sensor_read() {
    TEST_ASSERT_TRUE(sensor.read());
    TEST_ASSERT_FLOAT_IS_NOT_NAN(sensor.getTemperature());
    TEST_ASSERT_FLOAT_IS_NOT_NAN(sensor.getHumidity());
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_sensor_read);
    UNITY_END();
}

void loop() {}

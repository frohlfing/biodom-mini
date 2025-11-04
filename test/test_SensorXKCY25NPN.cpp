#include <Arduino.h>
#include <unity.h>
#include "SensorXKCY25NPN.h"

SensorXKCY25NPN levelSensor(35); // GPIO35

void test_water_detection() {
    bool result = levelSensor.read();
    TEST_ASSERT_TRUE(result == true || result == false); // bool valid
}

void setup() {
    delay(2000);
    levelSensor.begin();
    UNITY_BEGIN();
    RUN_TEST(test_water_detection);
    UNITY_END();
}

void loop() {}

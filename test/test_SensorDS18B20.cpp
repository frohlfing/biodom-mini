/**
 * @file test_main.cpp
 * @brief Unit-Test für die SensorDS18B20-Klasse mit Unity.
 * 
 * Testet die Sensorinitialisierung und Temperaturmessung.
 */

#include <Arduino.h>
#include <unity.h>
#include "SensorDS18B20.h"

SensorDS18B20 sensor(4); // GPIO4 für 1-Wire

void test_sensor_found() {
    TEST_ASSERT_NOT_EQUAL(1, sensor.getLastError()); // Sensor sollte gefunden sein
}

void test_temperature_valid() {
    TEST_ASSERT_TRUE(sensor.read());
    float temp = sensor.getTemperature();
    TEST_ASSERT_FALSE(isnan(temp));
    TEST_ASSERT(temp > -55.0 && temp < 125.0); // SensorDS18B20 Messbereich
}

void setup() {
    delay(2000); // Sensor-Stabilisierung
    UNITY_BEGIN();
    RUN_TEST(test_sensor_found);
    RUN_TEST(test_temperature_valid);
    UNITY_END();
}

void loop() {}

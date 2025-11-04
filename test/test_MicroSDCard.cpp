/**
 * @file test_MicroSDCard.cpp
 * Unity-Tests (Hardware-abhängig!). Läuft auf ESP32 mit SD-Karte.
 */

#include <Arduino.h>
#include <unity.h>
#include "MicroSDCard.h"

#define SD_CS_PIN 16
MicroSDCard sd(SD_CS_PIN, 1);

void test_begin_and_basic_write_read() {
  TEST_ASSERT_TRUE(sd.begin(2000000UL));
  const char* path = "/unittest.txt";
  sd.remove(path);
  TEST_ASSERT_FALSE(sd.exists(path));

  TEST_ASSERT_TRUE(sd.saveText(path, "unittest\n", false));
  TEST_ASSERT_TRUE(sd.exists(path));

  String s;
  TEST_ASSERT_TRUE(sd.readText(path, s));
  TEST_ASSERT_TRUE(s.indexOf("unittest") >= 0);

  TEST_ASSERT_TRUE(sd.remove(path));
  TEST_ASSERT_FALSE(sd.exists(path));
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_begin_and_basic_write_read);
  UNITY_END();
}

void loop() {}

/**
 * @file test_OLEDDisplaySH1106.cpp
 * Einfache Unit-Tests (funktional, nicht hardware-visualisiert).
 * Testet API-Aufrufe auf ESP32 mit angeschlossenem Display.
 *
 * Hinweis: Diese Tests verifizieren keine Pixel-Ausgabe, sondern rufen nur API auf.
 */

#include <Arduino.h>
#include <unity.h>
#include <U8g2lib.h>
#include "OLEDDisplaySH1106.h"

// Nutze simulierten U8g2 Konstruktor, der nicht zwingend Display braucht
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
OLEDDisplaySH1106 display(&u8g2);

void test_begin_and_basic_draws() {
  TEST_ASSERT_TRUE(display.begin());
  display.clear();
  display.drawText(0, 12, "test");
  display.drawTextf(0, 24, "num=%d", 42);
  display.drawProgress(0, 36, 100, 8, 50, 100);
  TEST_ASSERT_NOT_NULL(display.u8g2());
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_begin_and_basic_draws);
  UNITY_END();
}

void loop() {}

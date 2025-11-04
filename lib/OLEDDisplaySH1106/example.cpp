/**
 * @file example.cpp
 * Beispiel für OLEDDisplaySH1106 mit U8g2 (ESP32)
 *
 * Dieses Beispiel verwendet ein häufiges Setup:
 *  - SSD1306 128x64 via I2C (Change to your constructor if using SPI)
 *
 * Hinweis: Erzeuge eine U8G2-Instanz im Sketch und übergebe sie an den Wrapper.
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include "OLEDDisplaySH1106.h"

// Beispiel für SSD1306 I2C 128x64 (TwoWire default Wire)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
OLEDDisplaySH1106 display(&u8g2);

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("OLEDDisplaySH1106 Beispiel");

  if (!display.begin()) {
    Serial.println("Display init fehlgeschlagen");
    while (1) delay(1000);
  }
  // einfache Anzeige
  display.clear();
  display.drawText(0, 12, "Hello U8g2");
  display.display();

  delay(1000);

  // Fortschrittsbalken-Demo
  for (int v = 0; v <= 100; v += 10) {
    display.clear();
    display.drawTextf(0, 12, "Progress %d%%", v);
    display.drawProgress(0, 20, 120, 10, v, 100);
    display.display();
    delay(300);
  }

  // Beispiel: Zeichne Bitmap (Platzhalter; ersetze durch echtes Bitmap)
  const uint8_t icon[] = {
    0xFF,0x81,0x81,0xFF // sehr kleines Beispiel (8x4)
  };
  display.clear();
  display.drawBitmap(0, 32, 8, 4, icon);
  display.display();
}

void loop() {
  // Periodisch Status anzeigen
  display.clear();
  display.drawText(0, 12, "Biodom Mini");
  display.drawTextf(0, 26, "Heap: %u", ESP.getFreeHeap());
  display.display();
  delay(2000);
}

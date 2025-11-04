/**
 * @file example.cpp
 * Einfaches Beispiel für MicroSDCard.
 */

#include <Arduino.h>
#include "MicroSDCard.h"

#define SD_CS_PIN 16

MicroSDCard sd(SD_CS_PIN);

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("MicroSDCard Beispiel");

  if (!sd.begin(2000000UL)) {
    Serial.println("SD Init fehlgeschlagen");
    while (1) delay(1000);
  }
  Serial.println("SD initialisiert");

  // Schreibe Text
  if (sd.saveText("/log.txt", "Start: MicroSDCard Beispiel\n", true)) {
    Serial.println("Log geschrieben");
  } else {
    Serial.println("Log schreiben fehlgeschlagen");
  }

  // Schreibe Binary Beispiel (kleines Blob)
  const uint8_t demo[] = {0x42, 0x43, 0x44, 0x45};
  if (sd.saveBinary("/blob.bin", demo, sizeof(demo), false)) {
    Serial.println("Blob geschrieben");
  } else {
    Serial.println("Blob schreiben fehlgeschlagen");
  }
}

void loop() {
  // Lese und gib log.txt aus
  String content;
  if (sd.readText("/log.txt", content)) {
    Serial.println("log.txt Inhalt:");
    Serial.println(content);
  } else {
    Serial.println("log.txt nicht gefunden");
  }

  delay(5000);
}

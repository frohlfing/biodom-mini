/**
 * @file example.cpp
 * Einfaches Beispiel für MicroSDCard.
 *
 * Zeigt: init, saveText, saveBinary, streamWriteStart/Chunk/End, readBinary.
 */

#include <Arduino.h>
#include "MicroSDCard.h"

#define SD_CS_PIN 16 // GPIO16 für CS

// 1 Sektor Puffer (512 B). Für höhere Durchsatzanforderungen 2-4 wählen.
MicroSDCard sd(SD_CS_PIN, 1);

bool printChunk(const uint8_t* buf, size_t len, void* /*user*/) {
  size_t show = len < 32 ? len : 32;
  Serial.print("chunk ");
  for (size_t i = 0; i < show; ++i) {
    if (buf[i] < 0x10) Serial.print('0');
    Serial.print(buf[i], HEX);
    Serial.print(' ');
  }
  Serial.println();
  return true; // weiterlesen
}

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("MicroSDCard (SdFat-only) Beispiel - ESP32");

  if (!sd.begin(2000000UL)) {
    Serial.println("SD Init fehlgeschlagen");
    while (1) delay(1000);
  }
  Serial.println("SD initialisiert");

  // Text schreiben (atomar)
  if (sd.saveText("/log.txt", "Start: MicroSDCard Beispiel\n", true)) {
    Serial.println("Log geschrieben");
  } else {
    Serial.println("Log schreiben fehlgeschlagen");
  }

  // Binary schreiben (kleines Blob)
  const uint8_t demo[] = {0xDE,0xAD,0xBE,0xEF};
  if (sd.saveBinary("/blob.bin", demo, sizeof(demo), false)) {
    Serial.println("Blob geschrieben");
  } else {
    Serial.println("Blob schreiben fehlgeschlagen");
  }

  // Stream: Beispiel für größere Datei (hier nur Demo mit zwei Chunks)
  if (sd.streamWriteStart("/big.bin")) {
    uint8_t chunk1[512];
    memset(chunk1, 0xAA, sizeof(chunk1));
    sd.streamWriteChunk(chunk1, sizeof(chunk1));

    uint8_t chunk2[512];
    memset(chunk2, 0x55, sizeof(chunk2));
    sd.streamWriteChunk(chunk2, sizeof(chunk2));

    if (sd.streamWriteEnd()) Serial.println("big.bin geschrieben (stream)");
    else Serial.println("streamWriteEnd fehlgeschlagen");
  } else {
    Serial.println("streamWriteStart fehlgeschlagen");
  }

  // Lese Blob in Chunks und drucke ersten Bytes
  sd.readBinary("/blob.bin", printChunk, nullptr);
}

void loop() {
  // Zeige log.txt alle 5s
  String content;
  if (sd.readText("/log.txt", content)) {
    Serial.println("log.txt Inhalt:");
    Serial.println(content);
  } else {
    Serial.println("log.txt nicht gefunden");
  }
  delay(5000);
}

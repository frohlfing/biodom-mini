/**
 * @file example.cpp
 * Beispiel: captureToSD mit ArduCamMini2MPPlusOV2640 + MicroSDCard (ESP32)
 *
 * Anpassung: SD_CS_PIN in MicroSDCard-Instanz und camera CS ggf. ändern.
 */

#include <Arduino.h>
#include "ArduCamMini2MPPlusOV2640.h"
#include <MicroSDCard.h>

#define CAM_CS_PIN 17
#define SD_CS_PIN  16

ArduCamMini2MPPlusOV2640 cam(CAM_CS_PIN, 21, 22, 8000000UL, 512, 5000);
MicroSDCard sd(SD_CS_PIN, 1);

void setup() {
  Serial.begin(115200);
  delay(50);

  Serial.println("ArduCamMini2MPPlusOV2640 Beispiel");

  if (!cam.begin()) {
    Serial.println("Kamera init fehlgeschlagen");
    while (1) delay(1000);
  }
  Serial.println("Kamera init erfolgreich");

  if (!sd.begin(2000000UL)) {
    Serial.println("SD Init fehlgeschlagen");
    while (1) delay(1000);
  }
  Serial.println("SD init erfolgreich");

  // Beispiel: einfache Aufnahme in SD
  const char* path = "/photo001.jpg";
  if (cam.captureToSD(sd, path, 5000)) {
    Serial.println("Foto erfolgreich gespeichert");
  } else {
    Serial.println("Foto speichern fehlgeschlagen");
  }
}

void loop() {
  delay(2000);
}

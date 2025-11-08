/*
Funktionstest für den berührungslosen Füllstandsensor XKC-Y25-NPN
*/

#include <Arduino.h>

const int sensorPin = 35; // Sensor Output Pin

int sensorValue = 0;

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  sensorValue = digitalRead(sensorPin);
  if (sensorValue == LOW) {
    Serial.println(1);
  } else {
    Serial.println(0);
  }
  delay(300);
}
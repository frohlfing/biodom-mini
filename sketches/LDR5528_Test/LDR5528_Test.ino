/**
 * Funktionstest für den Fotowiderstand LDR5528
 */

#include <Arduino.h>

const int valLight = 1100;
const int valDark = 2500;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int val = analogRead(39);
  Serial.print("Raw:");
  Serial.print(val);
  Serial.print(",");
  int percent = map(val, valLight, valDark, 0, 100);
  percent = constrain(percent, 0, 100);
  Serial.print("Percent:");
  Serial.println(percent);
  delay(1000);
}
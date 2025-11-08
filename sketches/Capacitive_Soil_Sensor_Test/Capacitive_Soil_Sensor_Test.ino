/**
 * Funktionstest für den kapazitiven Bodenfeuchtesensor V1.2 (Capacitive Soil Sensor V.1.2, analog)
 */

#include <Arduino.h>

const int valWater = 1100;
const int valAir = 2500;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int val = analogRead(34);
  Serial.print("Raw:");
  Serial.print(val);
  Serial.print(",");
  
  int percent = map(val, valAir, valWater, 0, 100);
  percent = constrain(percent, 0, 100);
  Serial.print("Percent:");
  Serial.println(percent);

  delay(1000);
}
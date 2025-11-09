/**
 * Funktionstest für den Lichtsensor GY-302 BH1750
 */

#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  lightMeter.begin();
  Serial.println(F("BH1750 Test"));
}

void loop() {
  float lux = lightMeter.readLightLevel();
  Serial.print("Light (lux):");
  Serial.println(lux);
  delay(1000);
}

#include <Arduino.h>
#include "SensorLDR5528.h"

#if defined(ARDUINO_ARCH_ESP32)
  #define ADC_MAX_VALUE 4095
#else
  #define ADC_MAX_VALUE 1023
#endif

SensorLDR5528 ldr(36, 10000.0f, ADC_MAX_VALUE); // GPIO36 (ADC1_0 auf ESP32)

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("SensorLDR5528 Beispiel");
  ldr.begin();
}

void loop() {
  if (ldr.read()) {
    Serial.print("Raw: ");
    Serial.print(ldr.getRaw());
    Serial.print(" | Rldr(Ohm): ");
    Serial.print(ldr.getResistance());
    Serial.print(" | Lux(approx): ");
    float lx = ldr.getLux();
    if (isnan(lx)) Serial.println("N/A");
    else Serial.println(lx);
  } else {
    Serial.println("Lesefehler");
  }
  delay(1000);
}

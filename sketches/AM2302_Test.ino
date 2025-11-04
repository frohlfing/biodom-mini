/*
AM2302 (DHT22) Test
*/

#include <SimpleDHT.h>

int pinDHT22 = 13;
SimpleDHT22 dht22(pinDHT22);

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("AM2302 (DHT22) Test");
  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(2000);
    return;
  }
  Serial.print("Sample OK: ");
  Serial.print((float)temperature); Serial.print(" *C, ");
  Serial.print((float)humidity); Serial.println(" RH%");
  delay(2500); // DHT22 sampling rate is 0.5HZ.
}

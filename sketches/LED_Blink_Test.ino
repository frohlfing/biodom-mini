/*
LED Blink Test

Turns an LED on for one second, then off for one second, repeatedly.

This example code is in the public domain.
https://docs.arduino.cc/built-in-examples/basics/Blink/
*/

#define LED 5

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
}

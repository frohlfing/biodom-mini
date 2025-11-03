#include <Arduino.h>

#define LED 5

// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);
  //Serial.begin(9600);
  pinMode(LED, OUTPUT);
}

void loop() {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  delay(500);
  Serial.println("Hello world!");
  delay(500);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

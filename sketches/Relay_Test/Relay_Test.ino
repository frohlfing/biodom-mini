/*
Funktionstest für ein Relay.
*/

#include <Arduino.h>

uint8_t pinRelay = 25;

void setup() {
    pinMode(pinRelay, OUTPUT);
    Serial.begin(115200);
    digitalWrite(pinRelay, HIGH); // aus
 }

 void loop() {
    digitalWrite(pinRelay, HIGH); // aus
    Serial.println("aus");
    delay(5000); 
    digitalWrite(pinRelay, LOW);  // an   
    Serial.println("an");    
    delay(5000); 
}

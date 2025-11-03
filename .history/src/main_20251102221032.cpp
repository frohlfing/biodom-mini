// #include <Arduino.h>

// #define LED 5

// // put function declarations here:
// int myFunction(int, int);

// void setup() {
//   Serial.begin(115200);
//   //Serial.begin(9600);
//   pinMode(LED, OUTPUT);
// }

// void loop() {
//   digitalWrite(LED, HIGH);
//   delay(500);
//   digitalWrite(LED, LOW);
//   delay(500);
//   Serial.println("Hello world!");
//   delay(500);
// }

// // put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }

#include <Arduino.h>
// #include "AM2302.h"

//AM2302 sensor(13); // GPIO13

float a = 1.0;
float b = 1.0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starte AM2302-Test...");
}

// void loop() {
//     Serial.printf("Temperatur (°C):%.1f,Luftfeuchtigkeit (%):%.1f\r\n", a, b);
//     Serial.printf("temp:%.1f,feuchte:%.1f\r\n", a, b);
//     delay(2000); 
    // a *= 1.1;
    // b *= 1.2;
// }

void loop() {
Serial.print("Fehler ");
    Serial.print("17");
        Serial.print(": ");
        Serial.println("ffdsdf");

  delay(2000);
}
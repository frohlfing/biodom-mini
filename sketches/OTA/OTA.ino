/**
 * OTA (Over-The-Air)
 * 
 * Beispiel, wie das Hochladen über WLAN auf dem ESP32 ermöglicht wird.
 * Details hierzu `siehe /docs/OTA.md`.
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

// Konfiguration
//constexpr char HOSTNAME[] = "biodom-mini-dev";
//constexpr char WIFI_SSID[] = "WOLKE7";
//constexpr char WIFI_PASSWORD[] = "1234";
//constexpr char OTA_PASSWORD[] = "4321";
#include "secrets.h"

void setup() {
    Serial.begin(115200);

    // 1. Mit dem WLAN verbinden
    Serial.print("Verbinde mit WLAN...");
    WiFi.mode(WIFI_STA); // Station Mode (Client)
    WiFi.setHostname(HOSTNAME);  
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWLAN verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
    Serial.print("Host: ");
    Serial.println(WiFi.getHostname());

    // 2. OTA-Dienst starten
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();
    Serial.println("OTA-Dienst bereit");
    Serial.print("Warte auf OTA-Update...");
}

void loop() {
    // 3. OTA-Handler in der Hauptschleife aufrufen
    ArduinoOTA.handle();

    // sonstiger Code...
    Serial.print(".");
    delay(100);
}
#include <WiFi.h>
#include "OTA.h" // Unsere neue Bibliothek

// WLAN-Zugangsdaten
const char* ssid = "DEIN_WLAN_SSID";
const char* password = "DEIN_WLAN_PASSWORT";

// OTA-Konfiguration
const char* ota_hostname = "esp32-ota-test";
const char* ota_password = "admin";

// OTA-Objekt erstellen
OTA ota(ota_hostname, ota_password);

void setup() {
    Serial.begin(115200);
    Serial.println("Booting...");

    // Mit WLAN verbinden
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // OTA-Dienst starten
    ota.begin();
}

void loop() {
    ota.handle();

    delay(1000);
}
#include "OTA.h"

OTA::OTA(const char* hostname, const char* password)
    : _hostname(hostname), _password(password) {}

void OTA::begin() const {
    // Konfiguriere ArduinoOTA
    ArduinoOTA.setHostname(_hostname);
    ArduinoOTA.setPassword(_password);

    // Definiere Callbacks für verschiedene OTA-Events
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "Sketch";
        } else { // U_SPIFFS
            type = "Dateisystem";
        }
        // Hinweis: Wenn das Dateisystem aktualisiert wird, müssen alle offenen Dateien geschlossen werden.
        Serial.println("Starte OTA-Update für " + type);
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("\nOTA-Update beendet.");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Fortschritt: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA-Fehler[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Authentifizierung fehlgeschlagen");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin fehlgeschlagen");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Verbindung fehlgeschlagen");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Empfang fehlgeschlagen");
        else if (error == OTA_END_ERROR) Serial.println("Ende fehlgeschlagen");
    });

    ArduinoOTA.begin();
    Serial.println("OTA-Dienst bereit.");
}

void OTA::handle() {
    ArduinoOTA.handle();
}

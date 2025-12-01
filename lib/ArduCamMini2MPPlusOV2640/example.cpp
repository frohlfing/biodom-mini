/**
 * Beispiel zur Nutzung der ArduCamMini2MPPlusOV2640-Bibliothek.
 * 
 * Dieses Beispiel ist eine interaktive Demonstration der Kamerafunktionen.
 * Nach dem Start kann man über den seriellen Monitor Befehle an den ESP32 senden,
 * um Kameraeinstellungen zu ändern oder ein Foto aufzunehmen.
 * 
 * Die aufgenommenen Bilder werden als JPEG-Datenstrom über die serielle Schnittstelle
 * gesendet. Um diese Bilder anzuzeigen, wird eine spezielle Host-Anwendung benötigt,
 * wie z.B. [ArduCAM Host V2.0](https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/Software/Host-Debug-Tools).
 * 
 * --- Befehle ---
 * Senden Sie eines der folgenden Zeichen über den Seriellen Monitor:
 * '1' - '5': Nimm ein Bild in einer vordefinierten Auflösung auf.
 * 'e':      Wende den Sepia-Spezialeffekt an.
 * 'l':      Stelle den Weißabgleich auf "Bewölkt" ein.
 * 'n':      Setze alle Effekte auf "Normal" zurück.
 * 'h':      Zeige diese Hilfe-Nachricht erneut an.
 */

#include <Arduino.h>
#include "ArduCamMini2MPPlusOV2640.h"

// GPIO-Pin für den SPI Chip Select der Kamera
constexpr uint8_t CAM_CS_PIN = 17;

// Erstelle eine Instanz der Bibliotheksklasse
ArduCamMini2MPPlusOV2640 camera(CAM_CS_PIN);

void printHelp() {
    Serial.println("\n--- Interaktive Kamera-Steuerung ---");
    Serial.println("Senden Sie einen der folgenden Befehle:");
    Serial.println("  '1': Bild aufnehmen (320x240)");
    Serial.println("  '2': Bild aufnehmen (640x480)");
    Serial.println("  '3': Bild aufnehmen (1024x768)");
    Serial.println("  '4': Bild aufnehmen (1600x1200)");
    Serial.println("  'e': Sepia-Effekt aktivieren");
    Serial.println("  'l': Weißabgleich auf 'Bewölkt' setzen");
    Serial.println("  'n': Alle Effekte zurücksetzen");
    Serial.println("  'h': Diese Hilfe anzeigen");
    Serial.println("------------------------------------");
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {}
    // Warte auf die serielle Verbindung
    Serial.println("Starte ArduCAM-Funktionstest-Beispiel...");

    // Kamera initialisieren
    Serial.print("Initialisiere Kamera... ");
    if (camera.begin()) {
        Serial.println("erfolgreich!");
    } else {
        Serial.println("FEHLER!");
        Serial.println("Kamera nicht gefunden. Bitte Verkabelung prüfen. Programm wird angehalten.");
        while (true);
    }
    
    printHelp();
}

void loop() {
    // Prüfe, ob ein Befehl über die serielle Schnittstelle gesendet wurde
    if (Serial.available()) {
        char command = Serial.read();
        
        switch (command) {
            case '1':
                Serial.println("-> Befehl: Nimm Bild mit 320x240 auf...");
                // Starte die Aufnahme und streame die Daten direkt an die serielle Schnittstelle
                camera.captureAndStreamTo(ArduCamMini2MPPlusOV2640::S_320x240, Serial);
                Serial.println("-> Aufnahme abgeschlossen.");
                break;

            case '2':
                Serial.println("-> Befehl: Nimm Bild mit 640x480 auf...");
                camera.captureAndStreamTo(ArduCamMini2MPPlusOV2640::S_640x480, Serial);
                Serial.println("-> Aufnahme abgeschlossen.");
                break;
            
            case '3':
                Serial.println("-> Befehl: Nimm Bild mit 1024x768 auf...");
                camera.captureAndStreamTo(ArduCamMini2MPPlusOV2640::S_1024x768, Serial);
                Serial.println("-> Aufnahme abgeschlossen.");
                break;

            case '4':
                Serial.println("-> Befehl: Nimm Bild mit 1600x1200 auf...");
                camera.captureAndStreamTo(ArduCamMini2MPPlusOV2640::S_1600x1200, Serial);
                Serial.println("-> Aufnahme abgeschlossen.");
                break;

            case 'e':
                Serial.println("-> Befehl: Setze Spezialeffekt auf Sepia...");
                camera.setSpecialEffect(ArduCamMini2MPPlusOV2640::EFFECT_SEPIA);
                Serial.println("-> Effekt gesetzt.");
                break;

            case 'l':
                Serial.println("-> Befehl: Setze Weißabgleich auf Bewölkt...");
                camera.setLightMode(ArduCamMini2MPPlusOV2640::LM_CLOUDY);
                Serial.println("-> Weißabgleich gesetzt.");
                break;
            
            case 'n':
                Serial.println("-> Befehl: Setze alle Effekte auf Normal...");
                camera.setSpecialEffect(ArduCamMini2MPPlusOV2640::EFFECT_NORMAL);
                camera.setLightMode(ArduCamMini2MPPlusOV2640::LM_AUTO);
                // Hier könnten auch Sättigung, Kontrast etc. zurückgesetzt werden
                Serial.println("-> Effekte zurückgesetzt.");
                break;

            case 'h':
                printHelp();
                break;

            // Ignoriere Zeilenumbrüche und andere nicht zugewiesene Zeichen
            case '\r':
            case '\n':
                break;

            default:
                Serial.println("-> Unbekannter Befehl. Sende 'h' für Hilfe.");
                break;
        }
    }
}
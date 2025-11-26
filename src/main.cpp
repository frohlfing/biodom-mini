/**
 * @file main.cpp
 * @brief Hauptprogramm für das "Biodom Mini" Projekt.
 * 
 * Diese Datei enthält die Hauptlogik zur Steuerung des Gewächshauses.
 * Sie initialisiert alle Sensoren und Aktoren, liest periodisch Messwerte aus,
 * wendet die Steuerungslogik an und aktualisiert die Anzeige.
 * 
 * @version 1.0.5
 * @date 26.11.2025
 * @author Frank Rohlfing
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <SD.h>
#include <Wire.h>

// -- Einbinden der Konfiguration und der lokalen Bibliotheken --
#include "config.h"
#include "secrets.h"
#include "settings.h"
#include "SettingsManager.h"
#include "WebInterface.h"
#include "ArduCamMini2MPPlusOV2640.h"
#include "LED.h"
#include "MicroSDCard.h"
#include "OLEDDisplaySH1106.h"
#include "Relay.h"
#include "SensorAM2302.h"
#include "SensorBH1750.h"
#include "SensorCapacitiveSoil.h"
#include "SensorDS18B20.h"
#include "SensorXKCY25NPN.h"

// Splash Screen
#include "xbm/frank_128x64_xbm.h" // definiert das C-Array frank_128x64_bits[]

// Icons für das Dashboard
#include "xbm/thermometer_16x16_xbm.h"    // Icon für Raumtemperatur (S1)
#include "xbm/wet_16x16_xbm.h"            // Icon für Luftfeuchtigkeit (S1)
#include "xbm/engine_coolant_16x16_xbm.h" // Icon für Bodentemperatur (S2)
#include "xbm/moisture_16x16_xbm.h"       // Icon für Bodenfeuchte (S3)

// Icons, wenn Aktoren aktiv sind
#include "xbm/radiator_16x16_xbm.h"      // Heizung (A3) aktiviert (überlagert Icon für Bodentemperatur)
#include "xbm/air_16x16_xbm.h"           // Lüfter (A4) aktiviert (überlagert Icons für Raumtemperatur und Luftfeuchtigkeit)
#include "xbm/rainy_weather_16x16_xbm.h" // Pumpe (A5) aktiviert (überlagert Icon für Bodenfeuchte)
#include "xbm/dry_16x16_xbm.h"           // Vernebler (A6) aktiviert (überlagert Icon für Luftfeuchtigkeit)

// === Globale Objektinstanzen ===
// Hier werden für alle Hardware-Komponenten Objekte erstellt.
// Die Pin-Konfiguration wird aus der config.h geladen.

// --- Einstellungen ---
SettingsManager settingsManager;

// --- WebInterface ---
WebInterface webInterface;

// --- Sensoren ---
SensorAM2302 airSensor(PIN_AIR_SENSOR); // Raumtemperatur- und Luftfeuchtigkeitssensormodul AM2302 (S1)
SensorDS18B20 soilTempSensor(PIN_SOIL_TEMPERATUR_SENSOR); // Bodentemperatursensor DS18B20 (S2)
SensorCapacitiveSoil soilMoistureSensor(PIN_SOIL_MOISTURE_SENSOR, SOIL_MOISTURE_ADC_DRY, SOIL_MOISTURE_ADC_WET); // Kapazitiver Bodenfeuchtigkeitssensor v1.2 (S3)
SensorXKCY25NPN waterLevelSensor(PIN_WATER_LEVEL_SENSOR); // Berührungsloser Füllstandsensor XKC-Y25-NPN (S4)
SensorBH1750 lightSensor; // Lichtsensor GY-302 BH1750 (S5)

// --- Aktoren (Relais) ---
// Die Relais sind active-low, d.h. LOW schaltet sie ein.
Relay lamp1Relay(PIN_LAMP1_RELAY);    // Lampe 1 (A1)
Relay lamp2Relay(PIN_LAMP2_RELAY);    // Lampe 2 (A2)
Relay heaterRelay(PIN_HEATER_RELAY);  // Heizer (A3)
Relay fanRelay(PIN_FAN_RELAY);        // Lüfter (A4)
Relay pumpRelay(PIN_PUMP_RELAY);      // Wasserpumpe (A5)
Relay misterRelay(PIN_MISTER_RELAY);  // Vernebler (A6)

// --- Sonstige Peripherie ---
OLEDDisplaySH1106 display;            // 1.3 Zoll OLED Display, SSH1106 (Z1)
MicroSDCard sdCard(PIN_SPI_SD_CS);    // MicroSD SPI Kartenleser (Z2)
ArduCamMini2MPPlusOV2640 camera(PIN_SPI_CAMERA_CS); // ArduCAM Mini 2MP Plus, OV2640 (Z3)
LED debugLed(PIN_DEBUG_LED);          // LED (Z4)

// === Globale Variablen zur Zustandsspeicherung ===

// --- Sensorwerte ---
// Diese Variablen speichern die zuletzt gemessenen Werte, um sie im Programmablauf zu verwenden.
// Sie werden mit "Not a Number" (NAN) oder -1 initialisiert, um anzuzeigen, dass noch keine gültige Messung stattgefunden hat.
float currentAirTemp = NAN;         // Aktuelle Raumtemperatur in °C (S1)
float currentHumidity = NAN;        // Aktuelle Luftfeuchtigkeit in % (S1)
float currentSoilTemp = NAN;        // Aktuelle Bodentemperatur in °C (S2)
int currentSoilMoisture = -1;       // Aktuelle Bodenfeuchte in % (S3), -1 bedeutet "ungültig/nicht gemessen"
bool isWaterLevelOk = false;        // Zustand des Wasserfüllstands (S4), startet mit 'false' (kein Wasser), bis erste Messung erfolgt.
float currentLightLux = NAN;        // Aktuelle Lichtstärke in Lux (S5)

// --- Zeitsteuerung für nicht-blockierende Operationen ---
// Diese Variablen speichern den Zeitpunkt (in Millisekunden seit Start) der letzten Ausführung,
// um Aktionen in festen Intervallen ohne blockierende delay()-Aufrufe durchzuführen.
unsigned long lastSensorRead = 0;     // Zeitpunkt der letzten Sensormessung
unsigned long lastDisplayUpdate = 0;  // Zeitpunkt der letzten Display-Aktualisierung
unsigned long lastCameraCapture = 0;  // Zeitpunkt der letzten Kameraaufnahme
unsigned long lastBroadcastTime = 0;  // Zeitpunkt der letzten Broadcast-Nachricht

// === Zustands-Management für manuelle Steuerung ===

enum ControlMode { MODE_AUTO, MODE_ON, MODE_OFF };
ControlMode lamp1Mode = MODE_AUTO;  // Lampe 1 (A1)
ControlMode lamp2Mode = MODE_AUTO;  // Lampe 2 (A2)
ControlMode heaterMode = MODE_AUTO; // Heizer (A3)
ControlMode fanMode = MODE_AUTO;    // Lüfter (A4)
ControlMode pumpMode = MODE_AUTO;   // Wasserpumpe (A5)
ControlMode misterMode = MODE_AUTO; // Vernebler (A6)

// === Funktionsprototypen ===

void handleSensors();
void handleControlLogic();
void handleDisplay();
void handleCamera();
void handleBroadcast();

/**
 * Hält das Programm an.
 * 
 * Gibt vor dem Halt eine Meldung und optional weitere Details aus.
 * Zusätzlich wird die Debug-LED eingeschaltet.
 * @param message Die Fehlermeldung, die auf dem Display und seriell ausgegeben wird.
 * @param detail Details zur Fehlermeldung (optional).
 */
void halt(const char* message, const char* detail = "") {
    Serial.println(message);
    display.addLogLine(message);
    if (strlen(detail) > 0) {
        Serial.println(detail);
        display.addLogLine(detail);
    }
    Serial.println(F("System angehalten"));
    display.addLogLine(F("System angehalten"));

    // LED dauerhaft einschalten, um den Halt zu signalisieren.
    debugLed.on(); 

    // Endlosschleife, um das Programm anzuhalten
    while (true) {
        delay(100); // Prozessor entlasten
    }
}

/**
 * Gibt den Logeintrag aus.
 * 
 * Es wird etwas gewartet, damit der Eintrag auch auf dem Display gelesen werden kann, 
 * bevor eventuell der nächste Eintrag angezeigt wird.
 */
void log(const char* message) {
    Serial.println(message);
    display.addLogLine(message);
    // todo wieder auf 1000 setzen
    delay(100);
    //delay(1000);
}

/**
 * @brief Initialisierungsroutine, wird einmal beim Start ausgeführt.
 */
void setup() {
    // Zufallsgenerator mit einem unvorhersehbaren Wert von einem offenen Analog-Pin initialisieren 
    randomSeed(analogRead(36)); // der GPIO36 ist ungenutzt

    // --- Debug-LED, serielle Schnittstelle und Display initialisieren ---
    // (als erstes, damit Fehler so früh wie möglich angezeigt werden können)

    // Debug-LED (Z4) initialisieren
    // Die LED ist standardmäßig aus. 
    // Sie signalisiert, dass da System nicht gestartet werden konnte.
    debugLed.begin(); 

    // Serielle Schnittstelle initialisieren
    Serial.begin(115200);
    // Auf ein Zeichen des Seriellen Monitors warten (max. 5 Sekunden)
    const unsigned long startWait = millis();
    while (millis() - startWait < 5000) {
        if (Serial.available() > 0) {
            break; // Benutzer hat etwas gesendet, starte sofort
        }
        delay(100);
    }

    Serial.println("Biodom Mini startet");

    // I2C-Bus initialisieren
    // Für den ESP32 ist es eine gute Praxis, die SDA- und SCL-Pins explizit anzugeben.
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // Display (Z1) initialisieren
    if (!display.begin()) {  
        Serial.println("Kein Display! System angehalten.");
        // halt() sollte hier besser noch nicht aufgerufen werden, da die Funktion das Display voraussetzt
        debugLed.on(); 
        while (true) { delay(100); } // Endlosschleife, um das Programm anzuhalten
    }

    // Dateisystem initialisieren
    if (!LittleFS.begin(true)) {
        halt("LittleFS FEHLER", "Dateisystem korrupt");
    }

    Serial.println("--- LittleFS Dateisystem ---");
    const size_t totalBytes = LittleFS.totalBytes(); // Gesamtgröße der LittleFS-Partition
    const size_t usedBytes = LittleFS.usedBytes(); // den von Dateien belegter Speicherplatz
    const size_t freeBytes = totalBytes - usedBytes; // der freie Speicherplatz
    Serial.printf("Gesamtgröße:  %10u Bytes (%.2f KB)\n", totalBytes, totalBytes / 1024.0);
    Serial.printf("Belegt:       %10u Bytes (%.2f KB)\n", usedBytes, usedBytes / 1024.0);
    Serial.printf("Frei:         %10u Bytes (%.2f KB)\n", freeBytes, freeBytes / 1024.0);

    Serial.println("Dateien im LittleFS:");
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    if (!file) {
        Serial.println("  (Keine Dateien gefunden)");
    }
    while (file) {
        Serial.printf("  - %-20s (%u Bytes)\n", file.name(), file.size());
        file = root.openNextFile();
    }

    // Einstellungen laden
    if (!settingsManager.begin()) {
        halt("Settings FEHLER", "Einstellungen korrupt");
    }
    log("Einstellungen geladen");

    // --- Netzwerkdienste starten ---

    // Netzwerk initialisieren
    log("Verbinde mit WLAN...");
    WiFiClass::mode(WIFI_STA); // Station Mode (Client)
    WiFiClass::setHostname(HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // max. 20 * 500ms = 10 Sekunden lang versuchen NOLINT(*-static-accessed-through-instance)
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();
    if (WiFi.status() != WL_CONNECTED) { // NOLINT(*-static-accessed-through-instance)
        halt("WLAN FEHLER", "Verbindung fehlgeschlagen");
    }
    log("WLAN verbunden!");
    String ipMessage = "IP: " + WiFi.localIP().toString();
    log(ipMessage.c_str());
    String hostMessage = "Host: " + String(WiFiClass::getHostname());
    log(hostMessage.c_str());

    // OTA-Dienst starten
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();
    log("OTA-Dienst bereit");

    // NTP-Client initialisieren
    configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER); // started Hintergrund-Task für Zeitsynchronisation 
    log("Synchronisiere Zeit...");
    attempts = 0;
    tm timeInfo{};
    while (!getLocalTime(&timeInfo) && attempts < 20) { // max. 20 * 500ms = 10 Sekunden lang versuchen
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();
    if (!getLocalTime(&timeInfo)) {
        halt("NTP FEHLER", "Zeit nicht ermittelt");
    }
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", &timeInfo);
    log(timeString);

    // --- Restliche Hardware-Komponenten initialisieren ---

    log("Systemstart...");

    // S1
    if (!airSensor.begin()) { 
        halt("Luftsensor FEHLER", airSensor.getErrorMessage());
    } 
    log("Luftsensor OK");

    // S2
    if (!soilTempSensor.begin()) { 
        halt("Bodentemp. FEHLER"); 
    } 
    log("Bodentemperatur OK"); 

    // S3  
    if (!soilMoistureSensor.begin()) { 
        halt("Bodenfeuchte FEHLER"); 
    } 
    log("Bodenfeuchtesensor OK"); 

    // S4
    if (!waterLevelSensor.begin()) {
        halt("Wasserstand FEHLER"); 
    } 
    log("Wasserstandsensor OK");

    // S5 (I2C-Gerät)
    if (!lightSensor.begin()) { 
        halt("Lichtsensor FEHLER"); 
    } 
    log("Lichtsensor OK"); 

    // Sonstige Peripheriegeräte

    // Z2
    if (!sdCard.begin()) { 
        halt("SD-Karte FEHLER"); 
    } 
    log("SD-Karte OK"); 

    // Z3 (I2C-Gerät)
    if (!camera.begin()) { 
        halt("Kamera FEHLER"); 
    } 
    log("Kamera OK"); 

    // Relais initialisieren
    lamp1Relay.begin();  // A1
    lamp2Relay.begin();  // A2
    heaterRelay.begin(); // A3
    fanRelay.begin();    // A4
    pumpRelay.begin();   // A5
    misterRelay.begin(); // A6
    log("Aktoren initialisiert");

    // --- Webinterface initialisieren ---

    if (!webInterface.begin()) {
        halt("WebServer FEHLER", "WebServer nicht ok");
    }

     // Definiert die Logik, die ausgeführt wird, wenn eine Nachricht vom Browser ankommt.
    webInterface.onMessage = [](const String& msg) {
        // Parse die empfangene Nachricht als JSON-Dokument.
        JsonDocument doc;
        const DeserializationError error = deserializeJson(doc, msg);

        // Prüfe auf Parsing-Fehler.
        if (error) {
            Serial.print("WebSocket JSON-Fehler: ");
            Serial.println(error.c_str());
            return;
        }

        // Extrahiere den Nachrichtentyp.
        const char* type = doc["type"];
        if (!type) {
            Serial.println("WebSocket-Fehler: Nachricht ohne 'type'-Feld.");
            return;
        }

        // --- Verarbeite die Nachricht basierend auf ihrem Typ ---

        if (strcmp(type, "command") == 0) {
            // Ein BEFEHL zur Steuerung von Aktoren wurde empfangen.
            const char* command = doc["command"];
            const char* target = doc["target"];

            if (!command || !target) {
                Serial.println("WebSocket-Fehler: 'command' oder 'target' fehlt.");
                return;
            }

            Serial.printf("Befehl empfangen: %s, Ziel: %s\n", command, target);

            if (strcmp(command, "toggle") == 0) {
                if (strcmp(target, "lamp1") == 0) lamp1Relay.toggle();
                else if (strcmp(target, "lamp2") == 0) lamp2Relay.toggle();
                else if (strcmp(target, "heater") == 0) heaterRelay.toggle();
                else if (strcmp(target, "fan") == 0) fanRelay.toggle();
                else if (strcmp(target, "pump") == 0) pumpRelay.toggle();
                else if (strcmp(target, "mister") == 0) misterRelay.toggle();
                handleBroadcast(); // nch der manuellen Änderung sofort den neuen Systemstatus an alle Browser senden

            } else if (strcmp(command, "setMode") == 0) {
                const char* modeStr = doc["mode"];
                ControlMode mode;
                if (strcmp(modeStr, "auto") == 0) mode = MODE_AUTO;
                else if (strcmp(modeStr, "on") == 0) mode = MODE_ON;
                else if (strcmp(modeStr, "off") == 0) mode = MODE_OFF;
                else {
                    Serial.println("WebSocket-Fehler: 'mode' fehlerhaft.");
                    return;
                }
                if (strcmp(target, "lamp1") == 0) { lamp1Mode = mode; }
                else if (strcmp(target, "lamp2") == 0) { lamp2Mode = mode; }
                else if (strcmp(target, "heater") == 0) { heaterMode = mode; }
                else if (strcmp(target, "fan") == 0) { fanMode = mode; }
                else if (strcmp(target, "pump") == 0) { pumpMode = mode; }
                else if (strcmp(target, "mister") == 0) { misterMode = mode; }
                if (mode == MODE_ON) {
                    if (strcmp(target, "lamp1") == 0) lamp1Relay.on();
                    else if (strcmp(target, "lamp2") == 0) lamp2Relay.on();
                    else if (strcmp(target, "heater") == 0) heaterRelay.on();
                    else if (strcmp(target, "fan") == 0) fanRelay.on();
                    else if (strcmp(target, "pump") == 0) pumpRelay.on();
                    else if (strcmp(target, "mister") == 0) misterRelay.on();
                } else if (mode == MODE_OFF) {
                    if (strcmp(target, "lamp1") == 0) lamp1Relay.off();
                    else if (strcmp(target, "lamp2") == 0) lamp2Relay.off();
                    else if (strcmp(target, "heater") == 0) heaterRelay.off();
                    else if (strcmp(target, "fan") == 0) fanRelay.off();
                    else if (strcmp(target, "pump") == 0) pumpRelay.off();
                    else if (strcmp(target, "mister") == 0) misterRelay.off();
                }
                handleBroadcast(); // Sende sofort den neuen Modus-Status
            }

            // Hier könnten später weitere Befehle wie "setState" hin.

        } else if (strcmp(type, "saveSettings") == 0) {
            // EINSTELLUNGEN sollen gespeichert werden.
            // ... (diese Logik implementieren wir im nächsten Schritt) ...

        } else {
            Serial.printf("Unbekannter WebSocket-Nachrichtentyp: %s\n", type);
        }
    };

    // --- Initialisierung erfolgreich ---

    Serial.println("System gestartet.");

    // Splash-Screen kurz anzeigen (signalisiert, dass alles ok ist)
    display.showFullscreenXBM(128, 64, frank_128x64_xbm);
    delay(1000);

    // Initiales Auslesen aller Sensoren
    handleSensors();
}

/**
 * @brief Hauptschleife, wird kontinuierlich ausgeführt.
 */
void loop() {
    ArduinoOTA.handle();

    unsigned long currentTime = millis();

    // Nicht-blockierende Handler aufrufen

    if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
        lastSensorRead = currentTime;
        handleSensors();
    }

    if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        lastDisplayUpdate = currentTime;
        handleDisplay();
    }

    if (currentTime - lastCameraCapture >= CAMERA_CAPTURE_INTERVAL) {
        lastCameraCapture = currentTime;
        handleCamera();
    }

    // Periodischer Broadcast per WebSocket
    if (currentTime - lastBroadcastTime >= BROADCAST_INTERVAL) {
        lastBroadcastTime = currentTime;
        handleBroadcast();
    }

    // Steuerungslogik in jedem Zyklus ausführen, um schnell reagieren zu können
    handleControlLogic();

    // Update-Funktionen für zeitgesteuerte Komponenten aufrufen
    debugLed.update();
    fanRelay.update();
    pumpRelay.update();
    display.update();
}

/**
 * @brief Liest alle Sensoren aus und speichert die Werte in globalen Variablen.
 */
void handleSensors() {
    debugLed.on(); // LED an während des Lesens

    if (airSensor.read()) {
        currentAirTemp = airSensor.getTemperature();
        currentHumidity = airSensor.getHumidity();
    }
    
    if (soilTempSensor.read()) { 
        currentSoilTemp = soilTempSensor.getTemperature(); 
    }
    
    if (soilMoistureSensor.read()) { 
        currentSoilMoisture = soilMoistureSensor.getPercent(); 
    }
    
    if (lightSensor.read()) { 
        currentLightLux = lightSensor.getLux(); 
    }
    
    if (waterLevelSensor.read()) { 
        isWaterLevelOk = waterLevelSensor.isWaterDetected(); 
    }

    debugLed.off(); // LED aus nach dem Lesen

    Serial.printf("Luft: %.1f°C, %.1f%% | Boden: %.1f°C, %d%% | Licht: %.0f Lux | Wasser OK: %d\n", 
        currentAirTemp, 
        currentHumidity, 
        currentSoilTemp, 
        currentSoilMoisture, 
        currentLightLux, 
        isWaterLevelOk
    );
}

/**
 * @brief Aktualisiert das OLED-Display mit den aktuellen Sensorwerten.
 */
void handleDisplay() {
    // Wenn der Wasserstand niedrig ist, hat die Warnung absolute Priorität.
    if (!isWaterLevelOk) {
        display.showFullscreenAlert("WASSER\nNACHFUELLEN", true);
        return;
    }

    // --- Quadrant Oben Links: Raumtemperatur (S1) ---

    if (fanRelay.isOn()) {
        // Wenn der Lüfter läuft, zeige das Icon für den Lüfter.
        display.setDashboardIcon(OLEDDisplaySH1106::TOP_LEFT, 16, 16, air_16x16_xbm);
    } else {
        // Sonst zeige das Icon für die Raumtemperatur.
        display.setDashboardIcon(OLEDDisplaySH1106::TOP_LEFT, 16, 16, thermometer_16x16_xbm);
    }

    // Messwert anzeigen
    if (!isnan(currentAirTemp)) {
        display.setDashboardText(OLEDDisplaySH1106::TOP_LEFT, String(currentAirTemp, 1) + "C");
    } else {
        display.setDashboardText(OLEDDisplaySH1106::TOP_LEFT, "FEHLER");
    }

    // --- Quadrant Oben Rechts: Luftfeuchtigkeit (S1) ---

    if (misterRelay.isOn()) {
        // Wenn der Vernebler läuft, zeige das Icon für den Vernebler.
        display.setDashboardIcon(OLEDDisplaySH1106::TOP_RIGHT, 16, 16, dry_16x16_xbm);
    } else if (fanRelay.isOn()) {
        // Wenn der Lüfter läuft, zeige das Icon für den Lüfter (da er die Raumtemperatur UND die Luftfeuchtigkeit beeinflusst).
        display.setDashboardIcon(OLEDDisplaySH1106::TOP_RIGHT, 16, 16, air_16x16_xbm);
    } else {
        // Sonst zeige das Icon für die Luftfeuchtigkeit.
        display.setDashboardIcon(OLEDDisplaySH1106::TOP_RIGHT, 16, 16, wet_16x16_xbm);
    }

    // Messwert anzeigen
    if (!isnan(currentHumidity)) {
        display.setDashboardText(OLEDDisplaySH1106::TOP_RIGHT, String(currentHumidity, 0) + "%");
    } else {
        display.setDashboardText(OLEDDisplaySH1106::TOP_RIGHT, "FEHLER");
    }

    // Quadrant Unten Links: Bodentemperatur (S2)

    if (heaterRelay.isOn()) {
        // Wenn die Heizung an ist, zeige das Icon für den Heizer.
        display.setDashboardIcon(OLEDDisplaySH1106::BOTTOM_LEFT, 16, 16, radiator_16x16_xbm);
    } else {
        // Sonst zeige das Icon für die Bodentemperatur.
        display.setDashboardIcon(OLEDDisplaySH1106::BOTTOM_LEFT, 16, 16, engine_coolant_16x16_xbm);
    }
    
    // Messwert anzeigen
    if (!isnan(currentSoilTemp)) {
        display.setDashboardText(OLEDDisplaySH1106::BOTTOM_LEFT, String(currentSoilTemp, 1) + "C");
    } else {
        display.setDashboardText(OLEDDisplaySH1106::BOTTOM_LEFT, "FEHLER");
    }

    // Quadrant Unten Rechts: Bodenfeuchte (S3)
    
    if (pumpRelay.isOn()) {
        // Wenn die Pumpe läuft (Bewässerung aktiv), zeige das Icon für die Pumpe.
        display.setDashboardIcon(OLEDDisplaySH1106::BOTTOM_RIGHT, 16, 16, rainy_weather_16x16_xbm);
    } else {
        // Sonst zeige das Icon für die Bodenfeuchte.
        display.setDashboardIcon(OLEDDisplaySH1106::BOTTOM_RIGHT, 16, 16, moisture_16x16_xbm);
    }

    // Messwert anzeigen
    if (currentSoilMoisture != -1) {
        display.setDashboardText(OLEDDisplaySH1106::BOTTOM_RIGHT, String(currentSoilMoisture) + "%");
    } else {
        display.setDashboardText(OLEDDisplaySH1106::BOTTOM_RIGHT, "FEHLER");
    }

    // Nachdem alle Quadranten konfiguriert sind, das gesamte Dashboard auf dem Display zeichnen.
    display.showDashboard();  
}

/**
 * @brief Implementiert die Steuerungslogik für alle Aktoren.
 */
void handleControlLogic() {
    // Verweis auf die aktuellen Einstellungen holen
    const auto& settings = settingsManager.get();

    // Aktuelle Stunde ermitteln
    tm timeInfo{};
    if (!getLocalTime(&timeInfo)) {
        log("Fehler beim Abrufen der Zeit."); // dürfte nie vorkommen, da im Setup die Zeit synchronisiert wurde
        return;
    }
    const int currentHour = timeInfo.tm_hour;

    // -- Steuerung für die Lampe 1 (A1) --

    if (lamp1Mode == MODE_AUTO) {
        if (currentHour >= settings.lightOnHour && currentHour < settings.lightOffHour) { // todo je ein eigenes Zeitfenster für Lampe1 und Lampe 2 verwenden
            // Tageszeit
            if (!isnan(currentLightLux)) {
                // todo Logik einbauen
                lamp1Relay.on();
            } else {
                lamp1Relay.off();
            }
        } else {
            // Nachtzeit
            lamp1Relay.off();
        }
    }

    // -- Steuerung für die Lampe 2 (A2) --

    if (lamp2Mode == MODE_AUTO) {
        if (currentHour >= settings.lightOnHour && currentHour < settings.lightOffHour) { // todo je ein eigenes Zeitfenster für Lampe1 und Lampe 2 verwenden
            // Tageszeit
            if (!isnan(currentLightLux)) {
                // todo Logik einbauen
                lamp2Relay.on();
            } else {
                lamp2Relay.off();
            }
        } else {
            // Nachtzeit
            lamp2Relay.off();
        }
    }

    // -- Steuerung für den Heizer (A3) --

    if (heaterMode == MODE_AUTO) {
        if (currentSoilTemp < settings.soilTempTarget) {
            heaterRelay.on();
        }
        else if (currentSoilTemp > settings.soilTempTarget + 0.5f) {
            heaterRelay.off();
        }
    }

    // -- Steuerung für den Lüfter (A4) --

    if (fanMode == MODE_AUTO) {
        if (currentAirTemp > settings.airTempThresholdHigh && !fanRelay.isOn()) {
            fanRelay.pulse(settings.fanCooldownDurationMs);
        }
    }

    // Steuerung für die Pumpe (A5)

    if (pumpMode == MODE_AUTO) {
        if (isWaterLevelOk && !pumpRelay.isOn()) {
            if (currentSoilMoisture < settings.soilMoistureTarget && currentSoilMoisture != -1) {
                pumpRelay.pulse(settings.wateringDurationMs);
            }
        }
    }

    // -- Steuerung für den Vernebler (A6) --

    if (pumpMode == MODE_AUTO) {
        if (isWaterLevelOk) {
            if (currentHumidity < settings.humidityTarget) {
                misterRelay.on();
            }
            else if (currentHumidity > settings.humidityTarget + 5.0f) {
                misterRelay.off();
            }
        } else {
            misterRelay.off();
        }
    }
}

/**
 * @brief Nimmt ein Bild auf und speichert es auf der SD-Karte.
 */
void handleCamera() {
    Serial.println("Führe Kameraaufnahme aus...");
    display.showFullscreenAlert("FOTO...", false);

    char filename[30];
    sprintf(filename, "/img_%lu.jpg", millis());

    // KORREKTUR: Die neue Methode der sdCard-Klasse verwenden
    File imgFile = MicroSDCard::openFileForWriting(filename);
    if (!imgFile) {
        Serial.println("Konnte Datei auf SD-Karte nicht erstellen.");
        display.showFullscreenAlert("SD FEHLER", true);
        delay(2000);
        return;
    }

    // Bild aufnehmen und direkt in die Datei streamen
    if (camera.captureAndStreamTo(ArduCamMini2MPPlusOV2640::S_1600x1200, imgFile)) {
        Serial.printf("Bild erfolgreich gespeichert: %s\n", filename);
        display.showFullscreenAlert("FOTO OK", false);
    } else {
        Serial.println("Fehler bei der Bildaufnahme.");
        display.showFullscreenAlert("KAMERA FEHLER", true);
    }

    imgFile.close();
    delay(2000);
}

/**
 * @brief Sende den Status aller Sensoren und Aktoren an alle Clients
 */
void handleBroadcast() {
    // Erstelle das vollständige 'state'-Objekt für das Dashboard.
    JsonDocument doc;
    doc["type"] = "state"; // Der Typ, den das JS für das Dashboard erwartet

    const JsonObject values = doc["values"].to<JsonObject>();
    values["airTemp"] = currentAirTemp; // Raumtemperatur in °C (S1)
    values["humidity"] = currentHumidity; // Luftfeuchtigkeit in % (S1)
    values["soilTemp"] = currentSoilTemp; // Bodentemperatur in °C (S2)
    values["soilMoisture"] = currentSoilMoisture; // Bodenfeuchtigkeit in % (S3)
    values["waterLevelOk"] = isWaterLevelOk; // Wasserstand (S4)
    values["lightLux"] = currentLightLux; // Tageslicht in Lux (S5)

    // Aktor-Zustände
    values["lamp1On"] = lamp1Relay.isOn(); // Lampe 1 (A1)
    values["lamp2On"] = lamp2Relay.isOn(); // Lampe 2 (A2)
    values["heaterOn"] = heaterRelay.isOn(); // Heizer (A3)
    values["fanOn"] = fanRelay.isOn(); // Lüfter (A4)
    values["pumpOn"] = pumpRelay.isOn(); // Pumpe (A5)
    values["misterOn"] = misterRelay.isOn(); // Vernebler (A6)

    // Modus
    values["lamp1Mode"] = lamp1Mode == MODE_AUTO ? "auto" : lamp1Mode == MODE_ON ? "on" : "off" ;
    values["lamp2Mode"] = lamp2Mode == MODE_AUTO ? "auto" : lamp2Mode == MODE_ON ? "on" : "off";
    values["heaterMode"] =heaterMode == MODE_AUTO ? "auto" : heaterMode == MODE_ON ? "on" : "off";
    values["fanMode"] = fanMode == MODE_AUTO ? "auto" : fanMode == MODE_ON ? "on" : "off";
    values["pumpMode"] = pumpMode == MODE_AUTO ? "auto" : pumpMode == MODE_ON ? "on" : "off";
    values["misterMode"] = misterMode == MODE_AUTO ? "auto" : misterMode == MODE_ON ? "on" : "off";

    String jsonString;
    serializeJson(doc, jsonString);
    webInterface.broadcast(jsonString);
}
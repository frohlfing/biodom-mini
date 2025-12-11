/**
 * @file main.cpp
 * @brief Hauptprogramm für das "Biodom Mini" Projekt.
 *
 * Diese Datei enthält die Hauptlogik zur Steuerung des Gewächshauses.
 * Sie initialisiert alle Sensoren und Aktoren, liest periodisch Messwerte aus,
 * wendet die Steuerungslogik an und aktualisiert die Anzeige.
 *
 * @version 1.0.9
 * @date 11.12.2025
 * @author Frank Rohlfing
 *
 * Derzeit können die Kameraeinstellungen noch nicht gespeichert werden.
 */

#include <Arduino.h>
#include <WiFi.h>
// #include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <SD.h>
#include <Wire.h>

// -- Einbinden der Konfiguration und der lokalen Bibliotheken --
#include "config.h"
#include "secrets.h"
#include "settings.h"
#include "SettingsManager.h"
#include "WebUI.h"
#include "ArduCamOV2640.h"
#include "LED.h"
#include "MicroSDCard.h"
#include "OLEDDisplaySH1106.h"
#include "OTA.h"
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

// --- Einstellungen ---
SettingsManager settingsManager;

// --- Webinterface ---
WebUI webInterface;

OTA ota(HOSTNAME, OTA_PASSWORD);

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
ArduCamOV2640 camera(PIN_SPI_CAMERA_CS); // ArduCAM OV2640 Mini 2MP Plus (Z3)
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

// === Funktionsprototypen ===

void printFileSystemInfo();
bool readSensors();
void controlActors();
void controlCamera();
void updateDisplay();
void applyCameraSettings();
bool capture();
JsonObject getStateAsJson(JsonDocument& doc);
void broadcastState();
void broadcastSettings();
void handleWSClientConnect(AsyncWebSocketClient* client);
void handleWSMessage(AsyncWebSocketClient* client, const String& msg);
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

    delay(1000);
    ESP.restart();

    // // Endlosschleife, um das Programm anzuhalten
    // while (true) {
    //     delay(100); // Prozessor entlasten
    // }
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
    delay(LOG_DELAY);
}

/*
Symptom:
Nach einem "Soft Reset" (Software-Upload, OTA oder Reset-Knopf) bleibt die Bootsequenz bei "SD-Karte ok" hängen.
Wenn die Stromversorgung kurz aus und wieder eingeschaltet wird, startet der ESP ohne Probleme.

Die Ursache:
Wenn du einen "Soft Reset" machst (Software-Upload, OTA oder Reset-Knopf), wird die Stromversorgung nicht unterbrochen.

- I2C-Problem:
Ein I2C-Slave (z.B. der Kamerasensor oder das Display) könnte gerade ein Bit gesendet haben (SDA auf LOW ziehen), als
der Reset kam. Der ESP32 startet neu, sieht, dass die Datenleitung (SDA) blockiert ist ("Low"), und die Wire-Bibliothek
bleibt in einer Endlosschleife hängen, weil sie darauf wartet, dass der Bus frei wird.

- SPI-Konflikt:
Die SD-Bibliothek initialisiert den SPI-Bus oft mit sehr hohen Geschwindigkeiten. Wenn danach die camera.begin()
aufgerufen wird, ist der SPI-Bus vielleicht noch in einem Zustand oder Modus, den die ArduCAM-Bibliothek nicht erwartet
(oder die SD-Karte "hält" noch die MISO-Leitung).

Die Lösung:
Wir müssen in der setup() zwei Dinge tun:
1) I2C-Bus "freischaufeln": Bevor wir Wire.begin() aufrufen, wackeln wir manuell am Takt-Pin (SCL), bis alle Slaves die
Datenleitung (SDA) loslassen.
2) SPI-Bus beruhigen: Zwischen der Initialisierung der SD-Karte und der Kamera fügen wir eine Zwangspause ein und
stellen sicher, dass der Chip-Select (CS) der SD-Karte deaktiviert ist.
*/

/**
 * @brief Initialisierungsroutine
 */
void setup() {
    // Zufallsgenerator mit einem unvorhersehbaren Wert von einem offenen Analog-Pin initialisieren
    randomSeed(analogRead(36)); // der GPIO36 ist ungenutzt

    // --- Debug-LED, serielle Schnittstelle und Display initialisieren ---
    // (als erstes, damit Fehler so früh wie möglich angezeigt werden können)

    // Debug-LED (Z4) initialisieren
    // Die LED ist standardmäßig aus. Sie signalisiert, dass da System nicht gestartet werden konnte.
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

    // Alle CS-Pins auf HIGH setzen und SPI-Bus initialisieren
    pinMode(PIN_SPI_SD_CS, OUTPUT);
    digitalWrite(PIN_SPI_SD_CS, HIGH);
    pinMode(PIN_SPI_CAMERA_CS, OUTPUT);
    digitalWrite(PIN_SPI_CAMERA_CS, HIGH);
    SPI.begin();

    // Display (Z1) initialisieren
    if (!display.begin()) {
        Serial.println("Kein Display! System angehalten.");
        // halt() wird hier noch nicht aufgerufen werden, da die Funktion das Display voraussetzt
        debugLed.on();
        while (true) { delay(100); } // Endlosschleife, um das Programm anzuhalten
    }

    // Dateisystem initialisieren
    if (!LittleFS.begin(true)) {
        halt("LittleFS FEHLER", "Dateisystem korrupt");
    }
    //printFileSystemInfo();

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
    ota.begin();
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

    // todo friert den Bootvorgang scheinbar ein!
    //applyCameraSettings();
    //log("Kamera konfiguriert");

    // Relais initialisieren
    lamp1Relay.begin();  // A1
    lamp2Relay.begin();  // A2
    heaterRelay.begin(); // A3
    fanRelay.begin();    // A4
    pumpRelay.begin();   // A5
    misterRelay.begin(); // A6
    log("Aktoren initialisiert");

    // --- Webinterface initialisieren ---

    if (!webInterface.begin(&SD)) {
        halt("WebServer FEHLER", "WebServer nicht ok");
    }

    // Callback für neue Client-Verbindungen
    webInterface.onClientConnect = handleWSClientConnect;

    // Callback für eingehende Nachrichten
    webInterface.onMessage = handleWSMessage;

    // --- Initialisierung erfolgreich ---

    Serial.println("System gestartet.");

    // Splash-Screen kurz anzeigen (signalisiert, dass alles ok ist
    display.showFullscreenXBM(128, 64, frank_128x64_xbm);
    delay(1000);

    // Initiales Auslesen aller Sensoren
    readSensors();
}

/**
 * @brief Hauptschleife, wird kontinuierlich ausgeführt.
 */
void loop() {
    ota.handle();
    webInterface.cleanupClients();
    const unsigned long currentTime = millis();

    // Nicht-blockierende Handler aufrufen

    if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
        lastSensorRead = currentTime;
        readSensors();
    }

    if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
        lastDisplayUpdate = currentTime;
        updateDisplay();
    }

    if (currentTime - lastCameraCapture >= CAMERA_CAPTURE_INTERVAL) {
        lastCameraCapture = currentTime;
        capture();
    }

    // Periodischer Broadcast per WebSocket
    if (currentTime - lastBroadcastTime >= BROADCAST_INTERVAL) {
        lastBroadcastTime = currentTime;
        broadcastState();
    }

    // Steuerungslogik in jedem Zyklus ausführen, um schnell reagieren zu können
    controlActors();

    // Kamera-Logik
    controlCamera();

    // Update-Funktionen für zeitgesteuerte Komponenten aufrufen
    debugLed.update();
    fanRelay.update();
    pumpRelay.update();
    display.update();
}

// --- Händler ---

/**
 * @brief Wird aufgerufen, wenn sich ein neuer WebSocket-Client verbindet.
 * @param client Ein Pointer auf den neuen Client.
 */
void handleWSClientConnect(AsyncWebSocketClient* client) {
    // Sende dem neuen Client sofort den letzten bekannten Live-Status (nur an diesen Client)
    JsonDocument stateDoc;
    webInterface.sendTo(client, "state", getStateAsJson(stateDoc));

    // Sende dem neuen Client die aktuellen Einstellungen
    JsonDocument settingsDoc;
    const JsonObject values = settingsDoc.to<JsonObject>();
    settingsManager.serialize(values);
    webInterface.sendTo(client, "settings", values);
}

/**
 * @brief Verarbeitet eingehende Nachrichten von einem WebSocket-Client.
 * @param client Ein Pointer auf den Client, der die Nachricht gesendet hat.
 * @param msg Die empfangene Nachricht als String.
 */
void handleWSMessage(AsyncWebSocketClient* client, const String& msg) {
    // Parse die empfangene Nachricht als JSON-Dokument.
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, msg);
    if (error) {
        webInterface.consoleLog(client, "WebSocket JSON-Fehler: %s", error.c_str());
        return;
    }

    // Extrahiere den Nachrichtentyp.
    const char* type = doc["type"];
    if (!type) {
        webInterface.consoleLog(client, "WebSocket-Fehler: Nachricht ohne 'type'-Feld.");
        return;
    }

    // --- Aktor schalten ---

    if (strcmp(type, "setMode") == 0) {
        const JsonObject payload = doc["payload"];
        const char* modeStr = payload["mode"];
        ControlMode mode;
        if (strcmp(modeStr, "auto") == 0) mode = MODE_AUTO;
        else if (strcmp(modeStr, "on") == 0) mode = MODE_ON;
        else if (strcmp(modeStr, "off") == 0) mode = MODE_OFF;
        else {
            webInterface.consoleLog(client, "WebSocket-Fehler: 'mode' fehlerhaft.");
            return;
        }
        Settings& settings = settingsManager.getMutable();
        const char* targetStr = payload["target"];
        if (strcmp(targetStr, "lamp1") == 0) { settings.lamp1Mode = mode; }
        else if (strcmp(targetStr, "lamp2") == 0) { settings.lamp2Mode = mode; }
        else if (strcmp(targetStr, "heater") == 0) { settings.heaterMode = mode; }
        else if (strcmp(targetStr, "fan") == 0) { settings.fanMode = mode; }
        else if (strcmp(targetStr, "pump") == 0) { settings.pumpMode = mode; }
        else if (strcmp(targetStr, "mister") == 0) { settings.misterMode = mode; }
        settingsManager.save(); // Speichere die neuen Modi persistent
        controlActors(); // Wende den neuen Zustand sofort an (Relais schalten)
        broadcastSettings();
        broadcastState();
    }

    // --- Einstellungen speichern ---

    else if (strcmp(type, "saveSettings") == 0) {
        webInterface.consoleLog(client, "Befehl 'saveSettings' empfangen.");
        const JsonObject payload = doc["payload"].as<JsonObject>();
        if (payload) {
            settingsManager.deserialize(payload);
            applyCameraSettings(); // Neue Kamera-Einstellungen sofort anwenden
            if (settingsManager.save()) {
                broadcastSettings();
            } else {
                webInterface.consoleLog(client, "FEHLER: Konnte nicht speichern.");
            }
        }
    }

    // --- Bild aufnehmen ---

    else if (strcmp(type, "captureNow") == 0) {
        webInterface.consoleLog(client, "Manuelle Aufnahme...");
        if (!capture()) {
            webInterface.broadcast("captureFailed");
        }
    }

    // --- Bildliste anfordern ---

    else if (strcmp(type, "getImageList") == 0) {
        webInterface.consoleLog(client, "Befehl: Bildliste anfordern");

        if (!sdCard.isReady()) {
            webInterface.consoleLog(client, "Fehler: SD-Karte nicht bereit für Bildliste.");
            // Optional: Eine Fehlermeldung an den Client senden
            return;
        }

        JsonDocument doc2;
        JsonArray images = doc2.to<JsonArray>();

        sdCard.listDir("/", [&images](const String& filename, const size_t size) {
            if (filename.endsWith(".jpg")) {
                const JsonObject img = images.add<JsonObject>();
                img["path"] = "/" + filename;
                img["size"] = size;
            }
        });

        // Sende die Liste als 'imageList'-Nachricht an den anfragenden Client
        JsonDocument responseDoc;
        responseDoc["type"] = "imageList";
        responseDoc["payload"]["images"] = images;
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
    }

    // --- Alle Bilder löschen ---

    else if (strcmp(type, "deleteAllImages") == 0) {
        webInterface.consoleLog(client, "Befehl: Bilder löschen");
        // Payload extrahieren
        const JsonObject payload = doc["payload"];
        const char* password = payload["password"];

        if (password && strcmp(password, OTA_PASSWORD) == 0) {
            if (sdCard.deleteAllFilesInDir("/")) {
                webInterface.consoleLog(client, "SD-Karte bereinigt.");
                webInterface.broadcast("imageListCleared");
            } else {
                webInterface.consoleLog(client, "Fehler beim Löschen.");
            }
        } else {
            webInterface.consoleLog(client, "Falsches Passwort!");
        }
    }

    else {
        webInterface.consoleLog(client, "Unbekannter WebSocket-Nachrichtentyp: %s\n", type);
    }
}

// --- Hilfsfunktionen ---

/**
 * @brief Gibt Informationen über das Dateisystem (LittleFS) auf der seriellen Konsole aus.
 * Zeigt Gesamtgröße, belegten/freien Speicher und eine Liste der Dateien.
 */
void printFileSystemInfo() {
    Serial.println("--- LittleFS Dateisystem ---");
    const size_t totalBytes = LittleFS.totalBytes();
    const size_t usedBytes = LittleFS.usedBytes();
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
}

/**
 * @brief Liest alle Sensoren aus und speichert die Werte in globalen Variablen.
 */
bool readSensors() {
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
    return true;
}

/**
 * @brief Aktualisiert das OLED-Display mit den aktuellen Sensorwerten.
 */
void updateDisplay() {
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
void controlActors() {
    // Verweis auf die aktuellen Einstellungen holen
    const Settings& settings = settingsManager.get();

    // Aktuelle Stunde ermitteln
    tm timeInfo{};
    if (!getLocalTime(&timeInfo)) {
        Serial.println("Fehler beim Abrufen der Zeit."); // dürfte nie vorkommen, da im Setup die Zeit synchronisiert wurde
        return;
    }
    const int currentHour = timeInfo.tm_hour;

    // -- Steuerung für die Lampe 1 (A1) --

    if (settings.lamp1Mode == MODE_AUTO) {
        if (currentHour >= settings.light1OnHour && currentHour < settings.light1OffHour) {
            // Tageszeit
            if (!isnan(currentLightLux)) {
                if (currentLightLux < settings.light1LuxThresholdDark) {
                    // Zu dunkel -> Lampe AN
                    lamp1Relay.on();
                } else if (currentLightLux > settings.light1LuxThresholdBright) {
                    // Hell genug -> Lampe AUS
                    lamp1Relay.off();
                }
                // Dazwischen (im Hysterese-Bereich): Zustand beibehalten, nichts tun!
                // Dies verhindert das Oszillieren.
            } else {
                // Sensorausfall: Lampe im Zweifel AN
                lamp1Relay.on();
            }
        } else {
            // Nachtzeit
            lamp1Relay.off();
        }
    } else if (settings.lamp1Mode == MODE_ON) {
        lamp1Relay.on();
    } else {
        lamp1Relay.off();
    }

    // -- Steuerung für die Lampe 2 (A2) --

    if (settings.lamp2Mode == MODE_AUTO) {
        if (currentHour >= settings.light2OnHour && currentHour < settings.light2OffHour) {
            // Tageszeit
            if (!isnan(currentLightLux)) {
                if (currentLightLux < settings.light2LuxThresholdDark) {
                    // Zu dunkel -> Lampe AN
                    lamp2Relay.on();
                } else if (currentLightLux > settings.light2LuxThresholdBright) {
                    // Hell genug -> Lampe AUS
                    lamp2Relay.off();
                }
                // Dazwischen (im Hysterese-Bereich): Zustand beibehalten, nichts tun!
                // Dies verhindert das Oszillieren.
            } else {
                // Sensorausfall: Lampe im Zweifel AN
                lamp2Relay.on();
            }
        } else {
            // Nachtzeit
            lamp2Relay.off();
        }
    } else if (settings.lamp2Mode == MODE_ON) {
        lamp2Relay.on();
    } else {
        lamp2Relay.off();
    }

    // -- Steuerung für den Heizer (A3) --

    if (settings.heaterMode == MODE_AUTO) {
        if (currentSoilTemp < settings.soilTempTarget) {
            heaterRelay.on();
        }
        else if (currentSoilTemp > settings.soilTempTarget + 0.5f) {
            heaterRelay.off();
        }
    } else if (settings.heaterMode == MODE_ON) {
        heaterRelay.on();
    } else {
        heaterRelay.off();
    }

    // -- Steuerung für den Lüfter (A4) --

    if (settings.fanMode == MODE_AUTO) {
        if (currentAirTemp > settings.airTempThresholdHigh && !fanRelay.isOn()) {
            fanRelay.pulse(settings.fanCooldownDurationMs);
        }
    } else if (settings.fanMode == MODE_ON) {
        fanRelay.on();
    } else {
        fanRelay.off();
    }

    // Steuerung für die Pumpe (A5)

    if (settings.pumpMode == MODE_AUTO) {
        if (isWaterLevelOk && !pumpRelay.isOn()) {
            if (currentSoilMoisture < settings.soilMoistureTarget && currentSoilMoisture != -1) {
                pumpRelay.pulse(settings.wateringDurationMs);
            }
        }
    } else if (settings.pumpMode == MODE_ON) {
        if (isWaterLevelOk) {
            pumpRelay.on();
        }
        else {
            pumpRelay.off();
        }
    } else {
        pumpRelay.off();
    }

    // -- Steuerung für den Vernebler (A6) --

    if (settings.misterMode == MODE_AUTO) {
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
    } else if (settings.misterMode == MODE_ON) {
        if (isWaterLevelOk) {
            misterRelay.on();
        } else {
            misterRelay.off();
        }
    } else {
        misterRelay.off();
    }
}

/**
 * @brief Implementiert die Steuerungslogik für die Kamera.
 */
void controlCamera() {
    static uint32_t lastCaptureDay = 0;
    static int capturesToday = 0;
    tm timeInfo{};
    if (getLocalTime(&timeInfo)) {
        // Prüfe, ob ein neuer Tag begonnen hat
        if (timeInfo.tm_yday != lastCaptureDay) {
            lastCaptureDay = timeInfo.tm_yday;
            capturesToday = 0;
        }
        const auto& settings = settingsManager.get();
        if (settings.cameraCapturesPerDay > 0 && capturesToday < settings.cameraCapturesPerDay) {
            // Berechne das Intervall für heute
            uint32_t intervalSeconds = 86400 / settings.cameraCapturesPerDay;
            if (timeInfo.tm_hour * 3600 + timeInfo.tm_min * 60 >= capturesToday * intervalSeconds) {
                capture();
                capturesToday++;
            }
        }
    }
}

/**
 * @brief Nimmt ein Bild auf und speichert es auf der SD-Karte.
 * @return true bei Erfolg, false bei Fehler.
 */
bool capture() {
    display.showFullscreenAlert("FOTO...", false);

    char filename[30];
    tm timeInfo{};
    if (getLocalTime(&timeInfo)) {
        // Erzeuge einen Namen mit Zeitstempel (z.B. "/img_20251205_103000.jpg")
        sprintf(filename, "/img_%04d%02d%02d_%02d%02d%02d.jpg",
            timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
    } else {
        // Fallback, wenn Zeit nicht verfügbar ist
        sprintf(filename, "/img_%lu.jpg", millis());
    }

    if (!camera.saveToSD(filename)) {
        display.showFullscreenAlert("KAMERA FEHLER", true);
        delay(2000); // Fehlermeldung kurz anzeigen
        return false;
    }

    display.showFullscreenAlert("FOTO OK", false);
    webInterface.broadcast("newImage", "path", filename);
    delay(2000); // Erfolgsmeldung kurz anzeigen
    return true;
}

/**
 * @brief Wendet die in den Settings gespeicherten Kamera-Parameter auf die Hardware an.
 */
void applyCameraSettings() {
    // todo friert den Bootvorgang scheinbar ein!

    // const Settings& settings = settingsManager.get();
    // camera.setResolution(settings.cameraResolution);
    // camera.setLightMode(settings.cameraLightMode);
    // camera.setColorSaturation(settings.cameraSaturation);
    // camera.setBrightness(settings.cameraBrightness);
    // camera.setContrast(settings.cameraContrast);
    // camera.setSpecialEffect(settings.cameraSpecialEffect);
}

/**
 * @brief Erstellt ein JSON-Objekt mit dem aktuellen Status aller Sensoren und Aktoren.
 * @param doc Das JsonDocument, in dem das Objekt erstellt werden soll.
 * @return Ein JsonObject, das den aktuellen Systemstatus enthält.
 */
JsonObject getStateAsJson(JsonDocument& doc) {
    const JsonObject values = doc.to<JsonObject>();

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
    return values;
}

/**
 * @brief Sendet den Status aller Sensoren und Aktoren an alle Clients
 */
void broadcastState() {
    JsonDocument doc;
    webInterface.broadcast("state", getStateAsJson(doc));
}

/**
 * @brief Sendet die Einstellungen an alle Clients.
 */
void broadcastSettings() {
    JsonDocument doc;
    const JsonObject values = doc.to<JsonObject>(); // JsonDocument in ein JsonObject umwandeln
    settingsManager.serialize(values); // settingsManager füllt dieses Objekt
    webInterface.broadcast("settings", values);
}

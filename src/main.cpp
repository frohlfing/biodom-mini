/**
 * @file main.cpp
 * @brief Hauptprogramm für das "Biodom Mini" Projekt.
 * 
 * Diese Datei enthält die Hauptlogik zur Steuerung des Gewächshauses.
 * Sie initialisiert alle Sensoren und Aktoren, liest periodisch Messwerte aus,
 * wendet die Steuerungslogik an und aktualisiert die Anzeige.
 * 
 * @version 1.0.3
 * @date 13.11.2025
 * @author Frank Rohlfing
 */

#include <Arduino.h>
#include <SD.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

// -- Einbinden der Konfiguration und der lokalen Bibliotheken --
#include "config.h"
#include "secrets.h"
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

// Intervalle in Millisekunden
const unsigned long SENSOR_READ_INTERVAL = 5000;    // Sensoren alle 5 Sekunden lesen
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000; // Display jede Sekunde aktualisieren
const unsigned long CAMERA_CAPTURE_INTERVAL = 3600000; // Kamera alle 60 Minuten auslösen

// === Funktionsprototypen ===
void handleSensors();
void handleControlLogic();
void handleDisplay();
void handleCamera();

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
    delay(1000);
}

/**
 * @brief Initialisierungsroutine, wird einmal beim Start ausgeführt.
 */
void setup() {
    // Zufallsgenerator mit einem unvorhersehbaren Wert von einem offenen Analog-Pin initialisieren 
    randomSeed(analogRead(36)); // der GPIO36 ist ungenutzt

    // --- Debug-LED, serielle Schnittstelle und Display initialisieren ---
    // (als erstes, damit Fehler so früh wie möglich angezeigt werden können)

    // 1) Debug-LED (Z4) initialisieren
    // Die LED ist standardmäßig aus. 
    // Sie signalisiert, dass da System nicht gestartet werden konnte.
    debugLed.begin(); 

    // 2) Serielle Schnittstelle initialisieren
    Serial.begin(115200);
    //while (!Serial); // Auf serielle Verbindung warten
    Serial.println("Biodom Mini startet");

    // 3) I2C-Bus initialisieren
    // Für den ESP32 ist es eine gute Praxis, die SDA- und SCL-Pins explizit anzugeben.
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

    // 4) Display (Z1) initialisieren
    if (!display.begin()) {  
        Serial.println("Kein Display! System angehalten.");
        // halt() sollte hier besser noch nicht aufgerufen werden, da die Funktion das Display voraussetzt
        debugLed.on(); 
        while (true) { delay(100); } // Endlosschleife, um das Programm anzuhalten
    }

    // --- Netzwerk und OTA starten ---

    // 5) Netzwerk initialisieren
    log("Verbinde mit WLAN...");
    WiFi.mode(WIFI_STA); // Station Mode (Client)
    WiFi.setHostname(HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) { // max. 20 * 500ms = 10 Sekunden lang versuchen
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();
    if (WiFi.status() != WL_CONNECTED) {
        halt("WLAN FEHLER", "Verbindung fehlgeschlagen");
    }
    log("WLAN verbunden!");
    String ipMessage = "IP: " + WiFi.localIP().toString();
    log(ipMessage.c_str());
    String hostMessage = "Host: " + String(WiFi.getHostname());
    log(hostMessage.c_str());

    // 6) OTA-Dienst starten
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();
    log("OTA-Dienst bereit");

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
    // TODO: Die Zeit über das Internet via NTP (Network Time Protocol) abrufen.
    //  Hier als Dummy-Implementierung.
    int currentHour = 10; 

    // -- Steuerung für die Lampen (A1 und A2) --
    
    if (currentHour >= LIGHT_ON_HOUR && currentHour < LIGHT_OFF_HOUR) {
        // "Licht an"-Zeitfenster ist gegeben
        // Messwert des Tageslichtsensors abgreifen
        if (!isnan(currentLightLux)) {
            // Wir haben einen gültigen Messwert für das Tageslicht .
            if (currentLightLux > LIGHT_LUX_THRESHOLD_BRIGHT) {
                // Hell genug: Beide Lampen können aus bleiben.
                lamp1Relay.off();
                lamp2Relay.off();
            } else if (currentLightLux > LIGHT_LUX_THRESHOLD_DARK) {
                // Mittelhell: Eine Lampe zur Unterstützung einschalten.
                 if (random(2) == 0) {
                    // Fall 1: Wähle Lampe 1
                    lamp1Relay.on();
                    lamp2Relay.off();
                } else {
                    // Fall 2: Wähle Lampe 2
                    lamp1Relay.off();
                    lamp2Relay.on();
                }
            } else {
                // Zu dunkel: Volle Beleuchtung mit beiden Lampen.
                lamp1Relay.on();
                lamp2Relay.on();
            }
        } else {
            // Der Lichtsensor liefert keinen gültigen Wert.
            // Um die Pflanzen nicht zu gefährden, schalten wir im Zweifel beide Lampen ein.
            lamp1Relay.on();
            lamp2Relay.on();
        }
    } else {
        // Außerhalb des Zeitfensters: Beide Lampen sind immer aus, egal wie dunkel es ist.
        lamp1Relay.off();
        lamp2Relay.off();
    }

    // -- Steuerung für den Heizer (A3) --

    if (currentSoilTemp < SOIL_TEMPERATUR_TARGET) { 
        heaterRelay.on(); 
    } 
    else if (currentSoilTemp > SOIL_TEMPERATUR_TARGET + 0.5f) { 
        heaterRelay.off(); 
    }

    // -- Steuerung für den Lüfter (A4) --

    if (currentAirTemp > AIR_TEMPERATUR_THRESHOLD_HIGH && !fanRelay.isOn()) {
        fanRelay.pulse(FAN_COOLDOWN_DURATION_MS);
    }

    // Steuerung für die Pumpe (A5)

    if (isWaterLevelOk && !pumpRelay.isOn()) {
        if (currentSoilMoisture < SOIL_MOISTURE_TARGET && currentSoilMoisture != -1) {
            pumpRelay.pulse(WATERING_DURATION_MS);
        }
    }

    // -- Steuerung für den Vernebler (A6) --
    
    if (isWaterLevelOk) {
        if (currentHumidity < HUMIDITY_TARGET) { 
            misterRelay.on(); 
        } 
        else if (currentHumidity > HUMIDITY_TARGET + 5.0f) { 
            misterRelay.off(); 
        }
    } else {
        misterRelay.off();
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
    File imgFile = sdCard.openFileForWriting(filename);
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
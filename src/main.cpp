/**
 * @file main.cpp
 * @brief Hauptprogramm für das "Biodom Mini" Projekt.
 * 
 * Diese Datei enthält die Hauptlogik zur Steuerung des Gewächshauses.
 * Sie initialisiert alle Sensoren und Aktoren, liest periodisch Messwerte aus,
 * wendet die Steuerungslogik an und aktualisiert die Anzeige.
 * 
 * @version 1.0.1
 * @date 19.11.2025
 * @author Frank Rohlfing
 */

#include <Arduino.h>
#include <SD.h> // <-- KORREKTUR: Notwendig für den 'File'-Datentyp

// -- Einbinden der Konfiguration und der lokalen Bibliotheken --
#include "config.h"
#include "secrets.h"
#include "SensorAM2302.h"
#include "SensorBH1750.h"
#include "SensorCapacitiveSoil.h"
#include "SensorDS18B20.h"
#include "SensorXKCY25NPN.h"
#include "Relay.h"
#include "LED.h"
#include "OLEDDisplaySH1106.h"
#include "ArduCamMini2MPPlusOV2640.h"
#include "MicroSDCard.h"

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
Relay misterRelay(PIN_MISTER_RELAY);  // Luftbefeuchter (A6)

// --- Sonstige Peripherie ---
OLEDDisplaySH1106 display;            // 1.3 Zoll OLED Display, SSH1106 (Z1)
MicroSDCard sdCard(PIN_SPI_SD_CS);    // MicroSD SPI Kartenleser (Z2)
ArduCamMini2MPPlusOV2640 camera(PIN_SPI_CAMERA_CS); // ArduCAM Mini 2MP Plus, OV2640 (Z3)
LED debugLed(PIN_DEBUG_LED);          // LED (Z4)

// === Globale Variablen zur Zustandsspeicherung ===

// --- Sensorwerte ---
// Diese Variablen speichern die zuletzt gemessenen Werte, um sie im Programmablauf zu verwenden.
// Sie werden mit "Not a Number" (NAN) oder -1 initialisiert, um anzuzeigen, dass noch keine gültige Messung stattgefunden hat.
float currentAirTemp = NAN;         // Aktuelle Lufttemperatur in °C (S1)
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
 * @brief Initialisierungsroutine, wird einmal beim Start ausgeführt.
 */
void setup() {
    int print_delay = 1000;

    // --- Serielle Schnittstelle initialisieren ---

    Serial.begin(115200);
    while (!Serial); // Auf serielle Verbindung warten
    Serial.println("--- Biodom Mini startet ---");

    // --- Initialisierung der Hardware-Komponenten ---

    // Z4
    debugLed.begin(); // LED wird initialisiert (und ist standardmäßig aus)

    // Z1
    display.begin();  // TODO: Fehlerhandling wie bei den Sensoren hinzufügen
    display.addLogLine("Systemstart...");
    delay(print_delay);

    // Sensoren  

    // S1
    if (!airSensor.begin()) { 
      halt("Luftsensor FEHLER", airSensor.getErrorMessage());
    } 
    display.addLogLine("Luftsensor OK");
    delay(print_delay);

    // TODO ab hier die Sensoren usw. überarbeiten

    // S2
    if (!soilTempSensor.read()) { 
      halt("Bodentemp. FEHLER"); 
    } 
    display.addLogLine("Bodentemp. OK"); 
    delay(print_delay);

    // S3  
    soilMoistureSensor.begin();
    display.addLogLine("Bodenfeuchte OK");
    delay(print_delay);

    // S4
    waterLevelSensor.begin();
    display.addLogLine("Wasserstand OK");
    delay(print_delay);
    
    // S5
    if (!lightSensor.begin()) { 
      halt("Lichtsensor FEHLER"); 
    } 
    display.addLogLine("Lichtsensor OK"); 
    delay(print_delay);

    // Sonstige Peripheriegeräte

    // Z2
    if (!sdCard.begin()) { 
      halt("SD-Karte FEHLER"); 
    } 
    display.addLogLine("SD-Karte OK"); 
    delay(print_delay);

    // Z3
    if (!camera.begin()) { 
      halt("Kamera FEHLER"); 
    } 
    display.addLogLine("Kamera OK"); 
    delay(print_delay);

    // Relais initialisieren
    lamp1Relay.begin();  // A1
    lamp2Relay.begin();  // A2
    heaterRelay.begin(); // A3
    fanRelay.begin();    // A4
    pumpRelay.begin();   // A5
    misterRelay.begin(); // A6
    display.addLogLine("Aktoren OK");
    delay(print_delay);
    
    Serial.println("Initialisierung abgeschlossen.");
    delay(print_delay);

    display.addLogLine("Start abgeschlossen!");
    delay(print_delay);

    debugLed.off(); // LED aus nach erfolgreichem Start
    delay(2000); // Log kurz anzeigen
    
    // Initiales Auslesen aller Sensoren
    handleSensors();
}

/**
 * @brief Hauptschleife, wird kontinuierlich ausgeführt.
 */
void loop() {
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
    if (soilTempSensor.read()) { currentSoilTemp = soilTempSensor.getTemperature(); }
    if (soilMoistureSensor.read()) { currentSoilMoisture = soilMoistureSensor.getPercent(); }
    if (lightSensor.read()) { currentLightLux = lightSensor.getLux(); }
    isWaterLevelOk = waterLevelSensor.read();

    debugLed.off(); // LED aus nach dem Lesen

    Serial.printf("Luft: %.1f°C, %.1f%% | Boden: %.1f°C, %d%% | Licht: %.0f Lux | Wasser OK: %d\n",
                  currentAirTemp, currentHumidity, currentSoilTemp, currentSoilMoisture, currentLightLux, isWaterLevelOk);
}

/**
 * @brief Aktualisiert das OLED-Display mit den aktuellen Sensorwerten.
 */
void handleDisplay() {
    if (!isWaterLevelOk) {
        display.showFullscreenAlert("WASSER\nNACHFUELLEN", true);
    } else {
        display.setDashboardText(OLEDDisplaySH1106::TOP_LEFT, String(currentAirTemp, 1) + "C");
        display.setDashboardText(OLEDDisplaySH1106::TOP_RIGHT, String(currentHumidity, 1) + "%");
        display.setDashboardText(OLEDDisplaySH1106::BOTTOM_LEFT, String(currentSoilTemp, 1) + "C");
        display.setDashboardText(OLEDDisplaySH1106::BOTTOM_RIGHT, String(currentSoilMoisture) + "%");
        display.showDashboard();
    }
}

/**
 * @brief Implementiert die Steuerungslogik für alle Aktoren.
 */
void handleControlLogic() {
    // Annahme: Es ist eine RTC/NTP-Bibliothek verfügbar. Hier als Dummy-Implementierung.
    int currentHour = 10; 
    if (currentHour >= LIGHT_ON_HOUR && currentHour < LIGHT_OFF_HOUR) {
        lamp1Relay.on(); lamp2Relay.on();
    } else {
        lamp1Relay.off(); lamp2Relay.off();
    }

    if (currentSoilTemp < SOIL_TEMPERATUR_TARGET) { heaterRelay.on(); } 
    else if (currentSoilTemp > SOIL_TEMPERATUR_TARGET + 0.5f) { heaterRelay.off(); }

    if (currentAirTemp > AIR_TEMPERATUR_THRESHOLD_HIGH && !fanRelay.isOn()) {
        fanRelay.pulse(FAN_COOLDOWN_DURATION_MS);
    }
    
    if (isWaterLevelOk) {
        if (currentHumidity < HUMIDITY_TARGET) { misterRelay.on(); } 
        else if (currentHumidity > HUMIDITY_TARGET + 5.0f) { misterRelay.off(); }
    } else {
        misterRelay.off();
    }

    if (isWaterLevelOk && !pumpRelay.isOn()) {
        if (currentSoilMoisture < SOIL_MOISTURE_TARGET && currentSoilMoisture != -1) {
             pumpRelay.pulse(WATERING_DURATION_MS);
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
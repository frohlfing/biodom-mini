#pragma once 

/**
 * Zentrale Konfigurationsdatei für das Biodom Mini Projekt.
 */

// ------------------------------------------------------------
// GPIO-Pins für Sensoren, Aktoren und Peripheriegeräte
// ------------------------------------------------------------

// Sensoren
constexpr int PIN_AIR_SENSOR = 13;              // GPIO-Pin für den Raumtemperatur- und Luftfeuchtigkeitssensormodul AM2302 (S2)
constexpr int PIN_SOIL_TEMPERATUR_SENSOR = 4;   // GPIO-Pin für den Bodentemperatursensor DS18B20 (S3)
constexpr int PIN_SOIL_MOISTURE_SENSOR = 34;    // GPIO-Pin für den // Kapazitiver Bodenfeuchtigkeitssensor v1.2 (S4)
constexpr int PIN_WATER_LEVEL_SENSOR = 35;      // GPIO-Pin für den Füllstandsensor XKC-Y25-NPN (S5)

// I2C-Geräte (Kamera, Display und Lichtsensor GY-302 BH1750 (S6)
constexpr int PIN_I2C_SDA = 21;         // GPIO-Pin für I2C SDA (Data) Leitung
constexpr int PIN_I2C_SCL = 22;         // GPIO-Pin für I2C SCL (Clock) Leitung

// SPI-Geräte (Kamera und SD-Karte)
constexpr int PIN_SPI_MOSI = 23;        // GPIO-Pin für SPI MOSI Leitung
constexpr int PIN_SPI_MISO = 19;        // GPIO-Pin für SPI MISO Leitung
constexpr int PIN_SPI_SCK = 18;         // GPIO-Pin für SPI Clock Leitung
constexpr int PIN_SPI_SD_CS = 16;        // GPIO-Pin für SPI Chip Select der SD-Karte
constexpr int PIN_SPI_CAMERA_CS = 17;   // GPIO-Pin für SPI Chip Select der Kamera

// Aktoren
constexpr int PIN_LAMP1_RELAY = 14;     // GPIO-Pin für das Relais der Pflanzenlampe 1 (A1)
constexpr int PIN_LAMP2_RELAY = 27;     // GPIO-Pin für das Relais der Pflanzenlampe 2 (A2)
constexpr int PIN_HEATER_RELAY = 26;    // GPIO-Pin für das Relais der Heizmatte (A3)
constexpr int PIN_FAN_RELAY = 25;       // GPIO-Pin für das Relais des Lüfters (A4)
constexpr int PIN_PUMP_RELAY = 33;      // GPIO-Pin für das Relais der Wasserpumpe (A5)
constexpr int PIN_MISTER_RELAY = 32;    // GPIO-Pin für das Relais des Luftbefeuchters (A6)

// Sonstige Peripheriegeräte
constexpr int PIN_DEBUG_LED = 5;        // GPIO-Pin für die Debug-LED

// ------------------------------------------------------------
// NTP (Network Time Protocol) 
// ------------------------------------------------------------

const char* NTP_SERVER = "pool.ntp.org";    // NTP-Server
constexpr long GMT_OFFSET = 3600;           // Mitteleuropäische Zeit (MEZ) = UTC+1 = 3600 Sekunden für Zeitzone "Berlin"
constexpr int DAYLIGHT_OFFSET = 3600;       // Sommerzeit (MESZ) = UTC+2, also zusätzliche 3600 Sekunden (Sommer-/Winterzeit wird automatisch umgestellt)

// ------------------------------------------------------------
// Kalibrierung
// ------------------------------------------------------------

// Bodenfeuchte
constexpr int SOIL_MOISTURE_ADC_DRY = 2500; // ADC-Wert, wenn der Sensor in der Luft hängt (komplett trocken)
constexpr int SOIL_MOISTURE_ADC_WET = 1100; // ADC-Wert, wenn der Sensor in Wasser getaucht ist (komplett nass)

// ------------------------------------------------------------
// Intervalle
// ------------------------------------------------------------

// Sensoren lesen
constexpr unsigned long SENSOR_READ_INTERVAL = 5000;    // alle 5 Sekunden (in ms)

// Display aktualisieren
constexpr unsigned long DISPLAY_UPDATE_INTERVAL = 1000; // jede Sekunde (in ms)

// Kamera auslösen
constexpr unsigned long CAMERA_CAPTURE_INTERVAL = 3600000; // alle 60 Minuten (in ms)

// ------------------------------------------------------------
// Steuerungsparameter
// ------------------------------------------------------------

// Raumtemperatur (S1)
constexpr float AIR_TEMPERATUR_THRESHOLD_HIGH = 28.0; // Oberhalb dieser Temperatur (°C) wird der Lüfter eingeschaltet

// Luftfeuchtigkeit (S1)
constexpr float HUMIDITY_TARGET = 48.0; // Zielwert für Luftfeuchtigkeit (%), steuert Befeuchter

// Bodentemperatur (S2)
constexpr float SOIL_TEMPERATUR_TARGET = 19.0; // Zielwert für Bodentemperatur

// Bodenfeuchte (S3)
constexpr int SOIL_MOISTURE_TARGET = 50; // Zielwert für Bodenfeuchte in Prozent

// Wasserstand (S4)
constexpr bool WATER_LEVEL_TRIGGERED = false; // LOW/false = Wasser erkannt

// Betriebszeit für Lampen (A1 und A2)
constexpr int LIGHT_ON_HOUR = 01;    // Stunde, 0-23: Ab wann soll die Lampe angehen?
constexpr int LIGHT_OFF_HOUR = 02;  // Stunde, 0-23: Ab wann soll die Lampe ausgehen?

// Schwellwerte für Lichtsensor (S5), dass selbst in der Licht-An-Zeit eine oder beide Lampen aus bleiben dürfen 
constexpr float LIGHT_LUX_THRESHOLD_DARK   = 5.0f;  // Lux-Wert: ist das Tageslicht heller, bleibt eine Lampe aus
constexpr float LIGHT_LUX_THRESHOLD_BRIGHT = 15.0f; // Lux-Wert: ist das Tageslicht heller, bleiben beide Lampen aus

// Laufzeit des Lüfters (A4)
constexpr unsigned long FAN_COOLDOWN_DURATION_MS = 300000;  // 5 Minuten (in ms)

// Dauer der Bewässerung (A5)
constexpr unsigned long WATERING_DURATION_MS = 5000; // 5 Sekunden (in ms)

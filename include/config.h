#pragma once 

/**
 * Zentrale Konfigurationsdatei für das Biodom Mini Projekt.
 */

// ------------------------------------------------------------
// GPIO-Pins für Sensoren, Aktoren und Peripheriegeräte
// ------------------------------------------------------------

// Sensoren
constexpr int PIN_AIR_SENSOR = 13; // GPIO-Pin für den Raumtemperatur- und Luftfeuchtigkeitssensormodul AM2302 (S1)
constexpr int PIN_SOIL_TEMPERATUR_SENSOR = 4; // GPIO-Pin für den Bodentemperatursensor DS18B20 (S2)
constexpr int PIN_SOIL_MOISTURE_SENSOR = 34; // GPIO-Pin für den Kapazitiver Bodenfeuchtigkeitssensor v1.2 (S3)
constexpr int PIN_WATER_LEVEL_SENSOR = 35; // GPIO-Pin für den Füllstandsensor XKC-Y25-NPN (S4)

// I2C-Geräte (Kamera, Display und Lichtsensor GY-302 BH1750 (S5)
constexpr int PIN_I2C_SDA = 21; // GPIO-Pin für I2C SDA (Data) Leitung
constexpr int PIN_I2C_SCL = 22; // GPIO-Pin für I2C SCL (Clock) Leitung

// SPI-Geräte (Kamera und SD-Karte)
constexpr int PIN_SPI_MOSI = 23; // GPIO-Pin für SPI MOSI Leitung
constexpr int PIN_SPI_MISO = 19; // GPIO-Pin für SPI MISO Leitung
constexpr int PIN_SPI_SCK = 18; // GPIO-Pin für SPI Clock Leitung
constexpr int PIN_SPI_SD_CS = 16; // GPIO-Pin für SPI Chip Select der SD-Karte
constexpr int PIN_SPI_CAMERA_CS = 17; // GPIO-Pin für SPI Chip Select der Kamera

// Aktoren
constexpr int PIN_LAMP1_RELAY = 14; // GPIO-Pin für das Relais der Pflanzenlampe 1 (A1)
constexpr int PIN_LAMP2_RELAY = 27; // GPIO-Pin für das Relais der Pflanzenlampe 2 (A2)
constexpr int PIN_HEATER_RELAY = 26; // GPIO-Pin für das Relais der Heizmatte (A3)
constexpr int PIN_FAN_RELAY = 25; // GPIO-Pin für das Relais des Lüfters (A4)
constexpr int PIN_PUMP_RELAY = 33; // GPIO-Pin für das Relais der Wasserpumpe (A5)
constexpr int PIN_MISTER_RELAY = 32; // GPIO-Pin für das Relais des Luftbefeuchters (A6)

// Sonstige Peripheriegeräte
constexpr int PIN_DEBUG_LED = 5; // GPIO-Pin für die Debug-LED

// ------------------------------------------------------------
// NTP (Network Time Protocol) 
// ------------------------------------------------------------

const char* NTP_SERVER = "pool.ntp.org"; // NTP-Server
constexpr long GMT_OFFSET = 3600; // Mitteleuropäische Zeit (MEZ) = UTC+1 = 3600 Sekunden für Zeitzone "Berlin"
constexpr int DAYLIGHT_OFFSET = 3600; // Sommerzeit (MESZ) = UTC+2, also zusätzliche 3600 Sekunden (Sommer-/Winterzeit wird automatisch umgestellt)

// ------------------------------------------------------------
// Kalibrierung
// ------------------------------------------------------------

// Bodenfeuchte
constexpr int SOIL_MOISTURE_ADC_DRY = 2500; // ADC-Wert, wenn der Sensor in der Luft hängt (komplett trocken)
constexpr int SOIL_MOISTURE_ADC_WET = 1100; // ADC-Wert, wenn der Sensor in Wasser getaucht ist (komplett nass)

// ------------------------------------------------------------
// Flags
// ------------------------------------------------------------

constexpr bool WATER_LEVEL_TRIGGERED = false; // Wasserstand (S4) - LOW/false = Wasser erkannt

// ------------------------------------------------------------
// Intervalle
// ------------------------------------------------------------

constexpr unsigned long LOG_DELAY = 100; // Verzögerung in ms nach einem Log-Eintrag im Display (während des Bootens)
constexpr unsigned long SENSOR_READ_INTERVAL = 5000; // Intervall in ms, um Sensoren zu lesen (alle 5 Sekunden)
constexpr unsigned long DISPLAY_UPDATE_INTERVAL = 1000; // Intervall in ms, um das Display zu aktualisieren (jede Sekunde in ms)
constexpr unsigned long BROADCAST_INTERVAL = 2000; // Intervall in ms für ein WebSocket Broadcast (jede zweite Sekunde in ms)
constexpr unsigned long CAMERA_CAPTURE_INTERVAL = 3600000; // Intervall in ms, um ein Bild zu machen (alle 60 Minuten in ms)

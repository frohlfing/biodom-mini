/**
 * NTP (Network Time Protocol)
 *
 * Beispiel zur Zeitsynchronisation mit einem NTP-Server auf dem ESP32.
 * Details hierzu `siehe /docs/NTP.md`.
 */

#include <WiFi.h>
#include <time.h> // Die C-Standardbibliothek für Zeitfunktionen

// Konfiguration
//constexpr char HOSTNAME[] = "biodom-mini-dev";
//constexpr char WIFI_SSID[] = "WOLKE7";
//constexpr char WIFI_PASSWORD[] = "1234";
//constexpr char OTA_PASSWORD[] = "4321";
#include "secrets.h"

// NTP-Server und Zeitzonen-Konfiguration
const char* NTP_SERVER = "pool.ntp.org";    // NTP-Server
const long  GMT_OFFSET = 3600;              // Mitteleuropäische Zeit (MEZ) = UTC+1 = 3600 Sekunden für Zeitzone "Berlin"
const int   DAYLIGHT_OFFSET = 3600;         // Sommerzeit (MESZ) = UTC+2, also zusätzliche 3600 Sekunden

/**
 * Gibt die aktuelle, formatierte Zeit auf dem Seriellen Monitor aus.
 */
void printLocalTime() {
    struct tm timeinfo; // Eine Struktur, um die Zeitinformationen zu speichern

    // getLocalTime() füllt die 'timeinfo'-Struktur. Sie gibt 'false' zurück,
    // wenn die Zeit noch nicht synchronisiert wurde.
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Fehler beim Abrufen der Zeit.");
        return;
    }

    // Zeitinformationen formatieren und ausgeben
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", &timeinfo);
    Serial.println(timeString);
}

/**
 * Gibt die aktuelle Stunde des Tages zurück.
 * @return Die Stunde im 24-Stunden-Format (0-23).
 */
int getLocalHour() {
    struct tm timeinfo; // Eine Struktur, um die Zeitinformationen zu speichern

    // getLocalTime() füllt die 'timeinfo'-Struktur. Sie gibt 'false' zurück,
    // wenn die Zeit noch nicht synchronisiert wurde.
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Fehler beim Abrufen der Zeit.");
        return -1;
    }
    return timeinfo.tm_hour;
}

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

    // 2. NTP-Client initialisieren
    // configTime() startet den NTP-Synchronisationsprozess im Hintergrund.
    configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);

    // 3. Warten auf die erste Synchronisation
    Serial.print("Warte auf Zeitsynchronisation...");
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        delay(100);
        Serial.print(".");
    }
    Serial.println("\nZeit erfolgreich synchronisiert!");

    // Einmalig die synchronisierte Zeit ausgeben
    printLocalTime();

    Serial.print("Aktuelle Stunde: ");
    Serial.println(getLocalHour());
}

void loop() {
    // Gib die aktuelle Zeit jede Sekunde aus
    printLocalTime();
    delay(1000);
}
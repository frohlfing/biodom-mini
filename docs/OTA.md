# OTA (Over-The-Air)

OTA ermöglicht das Hochladen des Codes über WLAN. 

## 📦 Installation & Konfiguration

Für die Nutzung von OTA muss die `platformio.ini`-Datei wie folgt konfiguriert werden:

```ini
[env:esp32-ova]
; ...
upload_protocol = espota
upload_port = biodom-mini.local
upload_flags = --auth=${sysenv.OTA_PASSWORD}

```
Der `upload_port` ist die mDNS-Adresse (Hostname + Suffix ".local") oder IP-Adresse. 

In `upload_flags` wird das Passwort angegeben - am besten indirekt über die Umgebungsvariable `OTA_PASSWORD`, damit es nicht in der Datei steht und vom Git-Repository ausgeschlossen werden muss!

In VS Code unter Einstellungen/Arbeitsbereich/Features/Terminal können Umgebungsvariablen definiert werden.
Dabei wird `workspace.json` im `AppData`-Ordner des Benutzers geöffnet, in der folgender Eintrag hinterlegt werden muss:

```json
"settings": {
    "terminal.integrated.env.windows": {
        "OTA_PASSWORD": "dein-sicheres-passwort"
    }
}
```

## 💻 Beispiel

Das Beispiel verbindet sich mit dem WLAN und startet den OTA-Dienst. Anschließend gibt es minütlich eine Statusmeldung aus.

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

// Konfiguration
constexpr char WIFI_SSID[] = "WOLKE7";
constexpr char WIFI_PASSWORD[] = "wifi-password";
constexpr char HOSTNAME[] = "biodom-mini";
constexpr char OTA_PASSWORD[] = "ota-passwort";

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("\nOTA Beispiel-Sketch");

    // Mit dem WLAN verbinden
    Serial.print("Verbinde mit WLAN...");
    WiFi.mode(WIFI_STA);
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

    // 2. OTA-Dienst starten
    ArduinoOTA.setHostname(HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);
    ArduinoOTA.begin();
    Serial.println("OTA-Dienst bereit");
    
    Serial.print("Warte auf OTA-Update...");
}

void loop() {
    // 3. OTA-Handler in der Hauptschleife aufrufen
    ArduinoOTA.handle();

    // sonstiger Code...
    Serial.print(".");
    delay(1000);
}
```
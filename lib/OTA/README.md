# 📡 OTA (Over-the-Air)

Diese Bibliothek kapselt die Logik für Over-the-Air (OTA) Updates mit der `ArduinoOTA`-Bibliothek für den ESP32.

**Funktionsumfang:**

*   Einfache Initialisierung des OTA-Dienstes mit `begin()`.
*   Konfiguration von Hostname und Passwort über den Konstruktor.
*   Automatisches Handling der OTA-Events (Start, Ende, Fortschritt, Fehler) mit aussagekräftigen Log-Ausgaben auf der seriellen Konsole.

## 📦 Installation & Abhängigkeiten

Diese Bibliothek hat **keine externen Abhängigkeiten**. Sie verwendet die `ArduinoOTA`-Bibliothek, die bereits im ESP32 Arduino Core Framework enthalten ist.

## 🚀 Beispiel

Ein Beispiel für die Verwendung findest du in der `examples/ota_basic`-Datei innerhalb dieser Bibliothek.

## 📜 Lizenz

Diese Bibliothek ist ein Wrapper für die `ArduinoOTA`-Implementierung des ESP32 Arduino Core. Sie folgt deren Lizenzbedingungen.
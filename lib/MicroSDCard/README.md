# 📌 MicroSDCard

Diese Bibliothek kapselt die Nutzung des SD-Kartenlesers (MicroSD SPI Modules) für den ESP32

**Funktionsumfang:**

*   Einfache Initialisierung der SD-Karte mit `begin()`.
*   Methoden zum Erstellen, Löschen und Auflisten von Verzeichnissen.
*   Methoden zum Schreiben, Lesen, Anhängen, Umbenennen und Löschen von Dateien.
*   Bequemes Auslesen kleiner Textdateien direkt in einen `String`.
*   Effizientes Lesen großer Dateien direkt in einen `Stream`.
*   Abfrage von Karteninformationen wie Typ, Gesamtgröße und belegtem Speicher.

## 📦 Installation & Abhängigkeiten

Diese Bibliothek hat **keine externen Abhängigkeiten**. Sie verwendet die `SD`- und `FS`-Bibliotheken, die bereits im ESP32 Arduino Core Framework enthalten sind.

## ❕ Wichtige Hinweise

### Pin-Belegung

Das SD-Kartenmodul wird über SPI angebunden. Die Standard-Hardware-SPI-Pins des ESP32 sind:
*   **MOSI:** GPIO 23
*   **MISO:** GPIO 19
*   **SCK:** GPIO 18
*   **CS (Chip Select):** **Muss exklusiv sein!** Der Pin kann frei gewählt und dem Konstruktor übergeben werden (z.B. GPIO 16).

### 💽 Dateisystem

Die Bibliothek ist für SD-Karten ausgelegt, die mit einem **FAT16**- oder **FAT32**-Dateisystem formatiert sind. Dies ist der Standard für die meisten SD-Karten.

## 📜 Lizenz

Diese Bibliothek ist ein Wrapper für die SD-Karten-Implementierung des [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32). Sie folgt deren Lizenzbedingungen.

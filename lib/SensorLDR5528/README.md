# 📌 SensorLDR5528

Diese Bibliothek kapselt die Nutzung des Fotowiderstands LDR5528. 

Features:
* Roh-ADC-Lesung
* Berechnung des LDR-Widerstands (Ohm)
* Approximative Umrechnung in Lux (empirisch)

## 🗒️ Dateien

* `SensorLDR5528.h` / `SensorLDR5528.cpp` — Implementierung
* `example.cpp` — einfaches Beispiel
* `test/test_SensorLDR5528.cpp` — Unity-Test (Integrationstest)

## 📦 Abhängigkeiten

Keine zwingenden Bibliotheken.

Falls du Sensordaten auf ein Display anzeigen willst: Verwende U8g2 (U8g2_Arduino) für das OLED-Rendering; diese Bibliothek ist in deinem Projekt als lib_deps vorgesehen.

## ❕ Wichtige Hinweise

Beachte die ADC-Referenzspannung deiner Plattform (ESP32 typ. 3.3V, AVR typ. 5V). Ändere ggf. die verwendete Vcc-Konstante in adcToVoltage().

Für stabilere Messwerte in verrauschten Umgebungen erhöhe die Anzahl der Samples in `read().`

## 📐 Kalibrierung

Die Umrechnung Widerstand -> Lux ist nur ein Näherungsmodell. Für präzise Messungen kalibriere mit einem Referenz-Luxmeter und passe die Parameter `a` und `b` in `resistanceToLux()` an.

## 📜 Lizenz

MIT
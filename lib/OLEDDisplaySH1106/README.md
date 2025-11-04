# OLEDDisplaySH1106 - schlanker U8g2-Wrapper (für ESP32)

Leichter, dokumentierter Wrapper um U8g2 (olikraus). Vereinfacht typische
Display-Aufgaben (Text, formatierte Textausgabe, Progressbar, Bitmap) und
liefert zugleich Zugriff auf die rohe U8g2-Instanz für Spezialfälle.

Warum ein Wrapper statt direkter U8g2-Nutzung?
- Kürzere, klarere API für Standard-Aufgaben
- Bessere Lesbarkeit der Sketches
- Einheitliche Konventionen (z.B. atomare Display-Aktualisierung via display())

Features gegenüber "nackter" U8g2-Nutzung
- Vorschub: drawTextf für printf-ähnliche Textformatierung
- drawProgress: Fertige Fortschrittsleiste
- drawBitmap: 1-bit XBM-kompatible Bitmap-Hilfe
- setInverted / setContrast / setRotation: zentrale Steuerfunktionen
- Robuste Dokumentation: Mouse-Over-Dokumentation für jede Methode

## 📦 Installation

* Folgende Bibliothek muss in `platformio.ini` eingebunden werden:

```ini
lib_deps =
  olikraus/U8g2 @ ^2.36.15
```

## Hinweise & Tipps

- Fonts: U8g2 bringt viele eingebaute Fonts. Nutze `u8g2()->setFont(...)` über `u8g2()` falls du Spezialfonts brauchst.
- Performance: Verwende die Full-Buffer-Constructors (`*_F_*`) wenn dein ESP32 genug RAM hat (vereinfacht drawing), oder Page-buffered (`*_1_*`) um RAM zu sparen.
- Rotation: Manche Displays/Treiber verhalten sich hardwareabhängig bei Rotation; überprüfe deine physische Ausrichtung nach setRotation.
- Bitmap-Format: `drawBitmap` verwendet U8g2 XBM-Format (MSB-first, byte-per-row packing).
- Lizenz: Diese Wrapper-Lib ist MIT; U8g2 hat eigene Lizenzbedingungen (siehe upstream).

## 📜 Lizenz

Diese Bibliothek basiert auf [U8g2 by Oliver Kraus](https://github.com/olikraus/u8g2). Sie folgt deren Lizenzbedingungen ([2‑Clause BSD License](https://github.com/olikraus/u8g2?tab=License-1-ov-file), auch "New BSD" genannt; einzelne Fonts und Beispiel‑HALs können abweichende Lizenzen haben).

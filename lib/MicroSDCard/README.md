# MicroSDCard (SdFat-only, fokussiert auf ESP32)

SdFat-basiertes, gepuffertes SD-Wrapper-Modul. Ziel ist eine einfache API für robustes Schreiben/Lesen großer Binärdateien (z. B. Kamerabilder) und für Text/Logdateien. Die Implementierung nutzt double-buffered streaming auf Sektor-Ebene und atomare Writes (tmp -> rename).

Highlights gegenüber esp32/SD (Standard):
- Buffering / gepufferte I/O (SdFat): höhere Schreibleistung und geringere Latenz.
- Double-buffered Stream-API: entkoppelt Datenproduktion (z. B. Kamera) vom SD-Write. Die Standard-Puffergröße sit 512 B (1 Sektor). Für höhere Datenraten erhöhe bufferSectors (2–4 empfohlen).
- Atomare Datei-Operationen (tmp -> rename) reduzieren Risiko von Korruption.
- SdFat bietet optionale exFAT-Unterstützung, Dedicated SPI und feine Konfigurierbarkeit.
- Besser geeignet für hohe Datenraten, kontinuierliche Aufnahme oder Kameraintegration.

## 📦 Installation

* Folgende Bibliothek muss in `platformio.ini` eingebunden werden:

```ini
lib_deps =
  greiman/SdFat @ ^2.3.1
```

## 🔧 Hinweise zur Praxis

- Verwende 3.3V-kompatible SD-Module und kurze SPI-Leitungen; bei langen Leitungen SPI-Clock reduzieren.
- Für Kameras mit konstant hoher Datenrate: erhöhe bufferSectors auf 2–4.
- Unit-Tests sind hardware-abhängig; für CI ohne SD-Karte ist ein Mock erforderlich.

## 📜 Lizenz

Diese Bibliothek basiert auf [SdFat by Bill Greiman](https://github.com/greiman/SdFat). Sie folgt deren Lizenzbedingungen ([MIT](https://github.com/greiman/SdFat?tab=MIT-1-ov-file)).


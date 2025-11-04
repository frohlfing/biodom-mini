# MicroSDCard

Wrapper für MicroSD-Karten im SPI-Modus. Ziel ist eine robuste, RAM-schonende API für Text- und Binärdateien (z. B. Kamerabilder).

## Verhalten und Designentscheidungen

* Atomare Writes: Temporäre Datei + Rename, um Dateikorruption bei Schreibabbruch zu vermeiden.

* Chunked-IO: Große Dateien (Bilder) werden in Blöcken geschrieben/gelesen, um RAM zu sparen.

* SPI-Takt: Standard 4 MHz, bei langen Kabeln / Störungen auf 1–2 MHz reduzieren (Hinweis: lange SPI-Kabel verringern Stabilität).

* Dateisystem: SD library verwendet FAT; formatiere Karten als FAT32 (empfohlen für SDHC bis 32GB).

## Hinweise zur Performance & Karte

* Schreibdauer pro Bild hängt von Karte (Class), SPI-Implementierung und Kamera ab; in deiner Projektdoku sind empirische Abschätzungen dokumentiert (z. B. Class 4/10, typische JPG-Größen und erwartete Bildrate).

## Abhängigkeiten

* keine

## Lizenz

MIT
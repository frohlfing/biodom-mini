# ArduCamMini2MPPlusOV2640 - Wrapper für ArduCAM Mini 2MP Plus (OV2640)

Schlanker, dokumentierter Wrapper um die ArduCAM Arducam_mini Library (Upstream v1.0.2).
Fokus: ESP32, RAM-schonende Speicherung von JPEG-Aufnahmen direkt auf MicroSD (SdFat‑Backend).

## Wesentliche Features

- captureToSD(sd, path): direktes, atomares Speichern von Aufnahmen auf MicroSD (tmp->rename).
- captureStreaming(cb): streaming-basiertes Auslesen via Callback (für Netzwerk/Upload).
- captureToBuffer: liefert Bild in RAM (nur für Tests / kleine Bilder empfohlen).
- Chunked FIFO-Lesen (default 512 B) reduziert RAM‑Verbrauch.
- Konfigurierbare chunkSize und timeout.

## 📦 Installation

Diese Wrapper-Lib nutzt die offizielle ArduCAM Arducam_mini Bibliothek (Upstream). 

* Folgende Bibliotheken müssen in `platformio.ini` referenziert werden:

```ini
lib_deps =
  greiman/SdFat@^2.3.0
  https://github.com/ArduCAM/Arducam_mini.git#v1.0.2
```

Hinweis: greiman/SdFat dient als SD-Backend für die lokale MicroSDCard-Bibliothek.

## Tipps & Empfehlungen
- Verwende 3.3V-kompatible Kamera-Module und verkürze SPI-Leitungen bei hohen Datenraten.
- Erhöhe `chunkSize` (2×–4× 512B) bei sehr schnellen Kameraströmen, wenn SD‑Write in deinem Setup nicht Schritt hält.
- Nutze captureToSD als Standard-Flow; captureToBuffer nur für Debugging/Tests mit kleinen Bildern.

## 📜 Lizenz

Diese Bibliothek basiert auf [Arducam_mini by Arducam](https://github.com/ArduCAM/Arducam_mini). Sie folgt deren Lizenzbedingungen ([LGPL-2.1](https://github.com/ArduCAM/Arducam_mini/tree/v1.0.2?tab=License-1-ov-file)).

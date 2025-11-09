# 📌 JPGtoXBM

Diese Bibliothek konvertiert ein JPG-Bild (welches z.B. von der ArduCAM Mini aufgenommen wurde) in das XBM-Format, so dass es auf demOLED Displays SSH1106 angezeigt werden kann.

Weitere Details siehe:  
 [Proof Of Concept](./../../docs/JPGtoXBM.md)

🚧🚧🚧 Diese Bibliothek ist noch nicht fertiggestellt.🚧🚧🚧

## 📦 Installation & Abhängigkeiten

* Folgende Bibliothek muss in `platformio.ini` eingebunden werden:

```ini
lib_deps =
  bodmer/TJpg_Decoder @ ^1.1.0
```

(TODO: nicht diese: bitbank2/JPEGDEC @ ^1.8.4)

## 📜 Lizenz

Diese Bibliothek basiert auf [TJpg_Decoder by Bodmer](https://github.com/Bodmer/TJpg_Decoder). Sie folgt deren Lizenzbedingungen ([FreeBSD](https://github.com/Bodmer/TJpg_Decoder/blob/master/license.txt)).

TODO: Nicht diese:
Diese Bibliothek basiert auf [JPEGDEC by Larry Bank](https://github.com/bitbank2/JPEGDEC). Sie folgt deren Lizenzbedingungen ([Apache License 2.0](https://github.com/bitbank2/JPEGDEC/blob/master/LICENSE)).



# <img src="https://www.arduino.cc/wiki/static/arduino-app-76bd27c4ce7246825aceb8efe2871f7a.svg" alt="Beschreibung" width="28" height="28"> Arduino Sketches 

In diesem Verzeichnis liegen Sketche für die [Arduino IDE](https://www.arduino.cc/en/software/), die ich für den ersten Funktionstest der Bauteile verwendet habe.

Im Unterordner `JpgToBmp` liegen die Sketche, die ich für die Entwicklung der gleichnamigen Funktion verwendet habe.

## 📦 Installation

Folgende Bibliotheken werden für diese Sketche benötigt und müssen mittels Library Manager installiert werden:

* [Arducam_mini 1.0.1 by Arducam](https://github.com/ArduCAM/Arducam_mini) für die ArduCAM Mini 2MP Plus (mit OV2640-Chip)
* [BH1750 1.3.0 by claws (Christofer Laws)](https://github.com/claws/BH1750) für den Lichtsensor GY-302 (BH1750)
* [DallasTemperature 4.0.5 by Miles Burton](https://github.com/milesburton/Arduino-Temperature-Control-Library) für den Bodentemperatursensor DS18B20
* [OneWire 2.3.8 by Paul Stoffregen](https://github.com/PaulStoffregen/OneWire) für den Bodentemperatursensor DS18B20 (setzt DallasTemperature voraus)
* [SimpleDHT 1.0.15 by Winlin](https://github.com/winlinvip/SimpleDHT) für den Raumtemperatur- und Luftfeuchtigkeitssensor AM2302
* [U8g2 2.35.30 by oliver](https://github.com/olikraus/u8g2) für das 1.3 Zoll OLED Display SSH1106
* [TJpg_Decoder 1.1.0 by Bodmer](https://github.com/Bodmer/TJpg_Decoder) für den JPGtoXBM-Konverter


(TODO: Diese nicht: [JPEGDEC 1.8.4 by Larry Bank](https://github.com/bitbank2/JPEGDEC) für den JPGtoXBM-Konverter)

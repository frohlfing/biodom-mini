# <img src="https://www.arduino.cc/wiki/static/arduino-app-76bd27c4ce7246825aceb8efe2871f7a.svg" alt="Beschreibung" width="28" height="28"> Arduino Sketches 

In diesem Verzeichnis liegen Sketches für die [Arduino IDE](https://www.arduino.cc/en/software/), die ich für den ersten Funktionstest der Bauteile verwendet habe.

## 📦 Installation

Folgende Bibliothek werden für diese Sketches benötigt und müssen mittels Library Manager installiert werden:

* [Arducam_mini 1.0.1 by Arducam](https://github.com/ArduCAM/Arducam_mini) für die ArduCAM Mini 2MP Plus OV2640  
	Leider ist die Bibliothek so konzipiert, dass die Datei .\Arduino\libraries\Arducam_mini\src\memorysaver.h für die jeweilige Hardware angepasst werden muss.
	Für meine Kamera muss diese Definition einkommentiert werden:  
		`#define OV2640_MINI_2MP_PLUS`
* [BH1750 1.3.0 by claws (Christofer Laws)](https://github.com/claws/BH1750) für das GY-302 BH1750 Lichtsensormodul
* [DallasTemperature 4.0.5 by Miles Burton](https://github.com/milesburton/Arduino-Temperature-Control-Library) für den Bodentemperatursensors DS18B20
* [OneWire 2.3.8 by Paul Stoffregen](https://github.com/PaulStoffregen/OneWire) für den Sensor DS18B20 (setzt DallasTemperature vorraus)
* [SimpleDHT 1.0.15 by Winlin](https://github.com/winlinvip/SimpleDHT) für das AM2302-Modul
* [U8g2 2.35.30 by oliver](https://github.com/olikraus/u8g2) für 1.3 Zoll OLED Display SSH1106

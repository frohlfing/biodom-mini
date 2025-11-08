# 📌 SensorAM2302-Bibliothek

Diese Bibliothek kapselt die Nutzung des Raumtemperatur- und Luftfeuchtigkeitssensor AM2302, auch bekannt als DHT22. 
In der Projektdokumentation wird er von mir auch "Luftsensor" genannt. 

Der Sensor ist als Modul und auch "pur" ohne Modul erhältlich (was für die Klasse egal ist :-). 

## 📦 Installation

* Folgende Bibliothek muss in `platformio.ini` referenziert werden:

```ini
lib_deps =
  winlinvip/SimpleDHT @ ^1.0.15
```

## 📜 Lizenz

Diese Bibliothek basiert auf [SimpleDHT by Winlin](https://github.com/winlinvip/SimpleDHT). Sie folgt deren Lizenzbedingungen ([MIT](https://github.com/winlinvip/SimpleDHT/blob/master/LICENSE)).

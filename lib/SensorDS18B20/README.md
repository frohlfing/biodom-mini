# 📌 SensorDS18B20-Bibliothek

Diese Bibliothek kapselt die Nutzung des Bodentemperatursensors DS18B20.

## 📦 Installation

* Folgende Bibliothek muss in `platformio.ini` eingebunden werden:

```ini
lib_deps =
  milesburton/DallasTemperature @ ^4.0.5
```

## 🐞 Bugfix

Die Bibliothek verwendet DallasTemperature, die wiederum OneWire 2.3.8 von PaulStoffregen. Diese Version hat ein Bug: "extra tokens at end of #undef directive".

Zum fixen einfach die Datei [OneWire.cpp](./../../.pio/libdeps/debug/OneWire/OneWire.cpp) wie im [Issue 158](https://github.com/PaulStoffregen/OneWire/pull/158/commits/62c66991204bf07e7477df49fb06c2bb3b3fc363) beschrieben modifizieren.

## 📜 Lizenz

Diese Bibliothek basiert auf [DallasTemperature by Miles Burton](https://github.com/milesburton/Arduino-Temperature-Control-Library). Sie folgt deren Lizenzbedingungen ([MIT](https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/LICENSE)).

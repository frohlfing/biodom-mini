# SensorBH1750

Diese Bibliothek kapselt die Nutzung des Lichtsensors GY-302 (BH1750) in einer kompakten, leicht testbaren Klasse.

## 📦 Installation

* Folgende Bibliothek muss in `platformio.ini` eingebunden werden:

```ini
lib_deps =
  claws/BH1750 @ ^1.3.0
```

## Bugfix

Die Bibliothek verwendet BH1750, die wiederum OneWire 2.3.8 von PaulStoffregen. Diese Version hat ein Bug: "extra tokens at end of #undef directive".

Zum fixen einfach die Datei [OneWire.cpp](./../../.pio/libdeps/debug/OneWire/OneWire.cpp) wie im [Issure 158](https://github.com/PaulStoffregen/OneWire/pull/158/commits/62c66991204bf07e7477df49fb06c2bb3b3fc363) beschrieben modifizieren.

## Hinweis für Entwickler

Die Klasse verwendet standardmäßig den kontinuierlichen High-Resolution-Modus des BH1750 (Messdauer ~120 ms). Für sehr energiesparende Anwendungen müsste die Klasse erweitert werden, um Single-Measurement-Modi oder Power-Down/Up-Zyklen zu unterstützen.

## 📜 Lizenz

Diese Bibliothek basiert auf [BH1750 by claws (Christofer Laws)](https://github.com/claws/BH1750). Sie folgt deren Lizenzbedingungen ([MIT](https://github.com/claws/BH1750?tab=MIT-1-ov-file)).
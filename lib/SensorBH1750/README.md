# 📌 SensorBH1750

Diese Bibliothek kapselt die Nutzung des Lichtsensors GY-302 BH1750. 

BH1750 ist der eigentliche Chips des Sensors, GY-302 bezeichnet das eingesetzte Modul.

## 📦 Installation

* Folgende Bibliothek muss in `platformio.ini` eingebunden werden:

```ini
lib_deps =
  claws/BH1750 @ ^1.3.0
```

## ❕ Wichtige Hinweise

Die Klasse verwendet standardmäßig den kontinuierlichen High-Resolution-Modus des BH1750 (Messdauer ~120 ms). Für sehr energiesparende Anwendungen müsste die Klasse erweitert werden, um Single-Measurement-Modi oder Power-Down/Up-Zyklen zu unterstützen.

## 📜 Lizenz

Diese Bibliothek basiert auf [BH1750 by claws (Christofer Laws)](https://github.com/claws/BH1750). Sie folgt deren Lizenzbedingungen ([MIT](https://github.com/claws/BH1750?tab=MIT-1-ov-file)).
# SensorCapacitiveSoil

Diese Bibliothek kapselt die Nutzung des kapazitiven Bodenfeuchtesensors V1.2 (analog), auch bekannt als "Capacitive Soil Moisture Sensor".

## Funktionen

- Einfache Kapselung als Klasse
- Rohwert lesen (`getRaw`) und Feuchte in Prozent (`getPercent`)
- Laufzeit-Kalibrierung (`setCalibration`)

## Installation

Keine zusätzlichen Bibliotheken erforderlich. Einfach das Verzeichnis `SensorCapacitiveSoil` in den `lib/`-Ordner deines PlatformIO-Projekts legen.

## Kalibrierungshinweis

Die Standardwerte `dry = 2500` und `wet = 1100` sind Richtwerte. Für genaue Messergebnisse kalibriere den Sensor mit deinem Substrat und deiner Hardwareplattform (ADC-Auflösung beachten).

## Lizenz

Diese Bibliothek ist unter MIT-Lizenz nutzbar. Anpassungen und Verbesserungen sind willkommen.
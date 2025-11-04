# Relay

Ein kleiner, zuverlässiger Treiber für ein einzelnes Relais-Modul.

## Eigenschaften

* Einfacher Konstruktor: `Relay(pin, activeHigh = false, safeState = false)`

* Nicht-blockierendes Pulsieren: `pulse(durationMs)` + `update()`

* Ein / Aus / Toggle API

* Safety: `safeState` legt fest, in welchen Zustand Relais nach Reset oder Pulse zurückkehrt (Standard: Aus)

## Hinweise zur Hardware und Sicherheit

Relais schalten häufig Netzspannung oder hohe Ströme. Triff geeignete Schutzmaßnahmen:

Verwende Sicherungen und geeignete Kabelquerschnitte.

Bei induktiven Lasten (Pumpen, Motoren) zusätzliche Freilauf-/Snubber-Komponenten verwenden.

Achte auf ausreichende Isolation und Abstand auf der Platine.

Prüfe, ob dein Relaismodul invertierte Logik benutzt (häufig: LOW = Relais zieht an). Setze `activeHigh` entsprechend.

Beim Einsatz von Relais-Boards mit separater Spulenversorgung (JD-VCC) achte auf Jumper-Einstellungen.

## Testen

Die Unit-Tests sind hardware-abhängig. Auf einem echten Board (z. B. ESP32) kannst du `pio test -e <env>` verwenden.

Für Host- oder CI-Umgebungen ohne GPIOs müssen digitalWrite/pinMode/millis gemockt werden.

## Lizenz

MIT
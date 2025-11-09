# 📌 LED

Diese Bibliothek kapselt die Nutzung einer LED.

* Ein/Aus/Toggling

* Nicht-blockierendes Blinken (blink(onMs, offMs) + update())

* Unterstützung für aktive High/Low Verdrahtung

## ❕ Wichtige Hinweise

* `update()` muss regelmäßig (z. B. in loop()) aufgerufen werden, damit Blinken funktioniert.

* Verwende `activeHigh=false` für Boards oder Schaltungen mit invertierter Ansteuerung (z. B. Open-Drain, Low-aktives Relaismodul).

* Unit-Tests führen Delays aus; auf Host-Umgebungen ohne GPIO sind Mocks nötig.

## 📜 Lizenz

MIT
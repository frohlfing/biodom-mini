# 📚 Projektspezifische Bibliotheken

Dieses Verzeichnis ist für projektspezifische (lokale) Bibliotheken vorgesehen. PlatformIO kompiliert diese zu statischen Bibliotheken und verlinkt sie in die ausführbare Datei.

## 📦 Abhängigkeiten

Die benötigten Bibliotheken sind in `platformio.ini` mit der Variable `lib_deps` aufgeführt.

**Aktualisierung:**

So können die Projektabhängigkeiten und benutzerdefinierten Pakete aktualisiert werden:

```shell
pio pkg update
```

## ▶️ Beispielprogramm

Jede lokale Bibliothek enthält ein Anwendungsbeispiel namens `example.cpp`. Um dieses auszuführen, kann das Quellcode-Verzeichnis in `platformio.ini` temporär auf die Bibliothek umgebogen werden, z.B.:

```ini
src_dir = lib/AM2302
```

## 🧪 Unit-Test

Die Unit-Tests für die Bibliotheken befinden sich im Verzeichnis `test`. Sie können über das Labor-Icon der Aktivitätsleiste oder über die Kommandozeile gestartet  werden:

```bash
pio test -e debug
```

## 📖 Siehe auch...

[PlatformIO Library Management](https://docs.platformio.org/en/latest/librarymanager)

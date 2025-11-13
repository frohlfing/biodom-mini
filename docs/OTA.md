# OTA (Over-The-Air)

OTA ermöglicht das Hochladen des Codes über WLAN. 

## 📦 Installation & Konfiguration

Für ein OTA-Update müssen folgende Angaben in die `platformio.ini` eingetragen werden:

```ini
[env:esp32-ova]
; ...
upload_protocol = espota
upload_port = biodom-mini-dev.local
upload_flags = --auth=${sysenv.OTA_PASSWORD}

```
Der `upload_port` ist die mDNS-Adresse (Hostname + Suffix ".local") oder IP-Adresse. 

In `upload_flags` wird das Passwort angegeben - am besten indirekt über die Umgebungsvariable `OTA_PASSWORD`, damit es nicht in der Datei steht und vom Git-Repository ausgeschlossen werden muss!

In VS Code unter Einstellungen/Arbeitsbereich/Features/Terminal können Umgebungsvariablen definiert werden.
Dabei wird `workspace.json` im `AppData`-Ordner des Benutzers geöffnet, in der folgender Eintrag hinterlegt werden muss:

```json
"settings": {
    "terminal.integrated.env.windows": {
        "OTA_PASSWORD": "4321"
    }
}
```

**Anmerkung:**

Mit der Arduino IDE kann der Code auch drahtlos hochgeladen werden. 
Wenn das Programm läuft, ist der Port des Entwicklungsboard keine COM-Schnittstelle mehr, sondern ein Netzwerk-Anschluss (Menüpunkt Tools/Port, u.U. erst nach Neustart sichtbar):

![Tools > Port](./assets/OTA-Port.png)

## 💻 Beispiel-Code

s. [sketches/OTA](/sketches/OTA/OTA.ino)

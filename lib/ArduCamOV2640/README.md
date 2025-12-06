# 📌 ArduCamMini2MPPlusOV2640

Diese Bibliothek kapselt die Nutzung der ArduCAM OV2640 Mini 2MP Plus.

**Funktionsumfang:**

*   Einfache Initialisierung der Kamera mit `begin()`.
*   Aufnahme von JPEG-Bildern in verschiedenen Auflösungen (von 160x120 bis 1600x1200).
*   Streaming der Bilddaten über `Serial` oder Speichern auf einer SD-Karte.

## 📦 Installation & Abhängigkeiten

* In `platformio.ini` muss auf die originale ArduCAM-Bibliothek referenziert werden:

```ini
lib_deps =
  https://github.com/ArduCAM/Arducam_mini.git#v1.0.2
```

### `memorysaver.h` Workaround

Die ArduCAM-Bibliothek wurde so konzipiert, dass sie eigentlich an das vorhandene Kameramodul angepasst werden muss. Dies wird durch einen "Hack" vermieden: Ich definiere die notwendigen Makros (`_MEMORYSAVER_` und `OV2640_MINI_2MP_PLUS`) direkt in der Header-Datei, *bevor* die `ArduCAM.h` eingebunden wird. Dies stellt sicher, dass die Bibliothek sich korrekt für die Hardware konfiguriert.

## ❕ Wichtige Hinweise

### Pin-Belegung

Die Kamera benötigt sowohl eine SPI- als auch eine I2C-Schnittstelle.
*   **SPI:** Wird für die schnelle Übertragung der Bilddaten genutzt. Die Standard-Hardware-SPI-Pins des ESP32 sind:
    *   **MOSI:** GPIO 23
    *   **MISO:** GPIO 19
    *   **SCK:** GPIO 18
    *   **CS (Chip Select):** **Muss exklusiv sein!** Der Pin kann frei gewählt und dem Konstruktor übergeben werden (z.B. GPIO 17).
*   **I2C:** Wird für die Konfiguration des Kamerasensors verwendet. Die Standard-Pins sind:
    *   **SDA:** GPIO 21
    *   **SCL:** GPIO 22

### Stromversorgung

Die ArduCAM-Kamera kann, besonders während der Aufnahme eines hochauflösenden Bildes, Stromspitzen verursachen. Eine instabile Stromversorgung (z.B. direkt vom 3.3V-Pin eines USB-Programmierers) kann zu Abstürzen oder fehlerhaften Initialisierungen führen. Es wird eine stabile, externe 5V-Versorgung mit einem nachgeschalteten Spannungsregler oder einem Logic-Level-Shifter empfohlen.

### Grundprinzip der Datenspeicherung und des Streamings

Ein Mikrocontroller wie der ESP32 hat nur sehr wenig RAM (Arbeitsspeicher), typischerweise um die 512 KB. Ein Bild von der 2MP-Kamera (OV2640) kann aber leicht mehrere hundert Kilobyte groß sein (z.B. 1600x1200 JPEG ca. 150-300 KB). Es ist also unmöglich, das gesamte Bild auf einmal in den RAM des ESP32 zu laden.

Hier kommt das **ArduCAM-Modul** ins Spiel. Es besitzt einen eigenen **FIFO-Puffer (First-In, First-Out)**. Das ist ein kleiner, schneller Speicherchip direkt auf der Kamera-Platine (in diesem Fall 8 MByte).

Der Prozess läuft wie folgt ab:

1. **Aufnahmebefehl:** Der ESP32 sendet über SPI einen Befehl an das ArduCAM-Modul: "Nimm ein Bild auf!".
2. **Aufnahme in den Puffer:** Die Kamera (der OV2640-Sensor) nimmt das Bild auf und speichert die gesamten Bilddaten direkt in ihren eigenen schnellen 8-MByte-FIFO-Puffer. Der ESP32 ist in dieser Phase kaum involviert.
3. **Warten auf Abschluss:** Der ESP32 wartet auf ein Signal vom ArduCAM-Modul, das anzeigt: "Bildaufnahme abgeschlossen, Daten sind im Puffer bereit."
4. **Datenstrom auslesen:** Jetzt beginnt der "Streaming"-Teil. Der ESP32 kann die Bilddaten nicht auf einmal anfordern. Stattdessen liest er sie in kleinen, handhabbaren Paketen (Chunks) aus dem FIFO-Puffer aus. Er sagt dem ArduCAM-Modul quasi: "Gib mir die nächsten 256 Bytes".
5. **Verarbeitung der Pakete:** Sobald der ESP32 ein Paket empfangen hat, kann er es sofort weiterverarbeite, z.B. über die serielle Schnittstelle weiterleiten oder direkt in die Datei auf einer SD-Karte schreiben.
6. **Wiederholung:** Dieser Prozess (Lesen eines Pakets, Schreiben des Pakets) wird so lange wiederholt, bis der gesamte FIFO-Puffer ausgelesen ist.

**Fazit:**

Man arbeitet mit einem **Datenstrom**. Das Bild wird nie als Ganzes im RAM des ESP32 gehalten. Es fließt stückchenweise vom Kamera-Puffer über den ESP32 auf die serielle Schnittstelle oder direkt auf die SD-Karte. Deshalb kann man im `loop()` nicht einfach `image = camera.capture()` und `sd.save(image)` machen, weil image viel zu groß für den Speicher wäre. Die Logik muss den Datenstrom in kleinen Teilen verarbeiten.

### Host Debug Tool

Werden die Daten über die Serielle Schnittstelle gesendet, kann z.B. [ArduCAM Host V2.0](https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/Software/Host-Debug-Tools/) diesen Datenstrom empfangen und als Bild anzeigen.

## 📜 Lizenz

Diese Bibliothek basiert auf [Arducam_mini by Arducam](https://github.com/ArduCAM/Arducam_mini). Sie folgt deren Lizenzbedingungen ([LGPL-2.1](https://github.com/ArduCAM/Arducam_mini/tree/v1.0.2?tab=License-1-ov-file)).

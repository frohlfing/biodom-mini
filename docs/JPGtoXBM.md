# Teilprojekt: JPGtoXBM

## Anforderung

Ich möchte mit der Kamera (ArduCAM Mini 2MP Plus OV2640) ein Jpg aufnehmen. Der ESP32 DevKitC V2 soll das Bild in ein Byte-Array umwandeln, so dass es auf dem 1.3 Zoll OLED Display SSH1106 angezeigt werden kann.
Das Display hat 128x64 Pixel, ist monochrom, 1 Bit pro Pixel.

### Grundsätzliche Konzeption: Die Daten-Pipeline

Die Herausforderung ist Transformation der Bilddaten. Diese ist rechenintensiv. Der ESP32 ist zwar schnell, aber man sollte keine Wunder erwarten. Die Konvertierung eines hochauflösenden Fotos wird einige Sekunden dauern. Für die Anzeige von Standbildern ist das aber völlig akzeptabel. 

Wir müssen eine Kette von Verarbeitungsschritten aufbauen, eine sogenannte Pipeline. Diese sieht konzeptionell so aus:

**JPG-Stream -> [ JPG-Dekompression ] -> [ Skalierung & Graustufen ] -> [ Dithering ] -> [ XBM-Formatierung ] -> XBM-Stream**

Die ArduCam oder die SD-Karte liefert den JPG-Datenstrom an die Pipeline.

1.  **Verarbeitungsschritt 1: JPG-Dekompression:**  
Die JPG-Daten sind komprimiert. Der erste Schritt muss sein, sie zu dekomprimieren, um an die rohen Pixelinformationen (Farbe für jeden einzelnen Bildpunkt) zu gelangen.
Das erledigt die Bibliothek [TJpg_Decoder by Bodmer](https://github.com/Bodmer/TJpg_Decoder).

(TODO: nicht diese: [JPEGDEC by Larry Bank](https://github.com/bitbank2/JPEGDEC))

Sie ist für genau diesen Zweck für zur Ausführung auf Mikrocontroller geschrieben. Um den verfügbaren RAM zu schonen, arbeitet die Bibliothek in kleinen Blöcken (sog. MCUs, oft 8x8 oder 16x16 Pixel). Sie dekomprimiert einen Block, und übergibt diesen an eine **Callback-Funktion**. In dieser Callback-Funktion führen wir dann unsere nachfolgenden Schritte (Skalierung, Dithering, Formatierung) durch, bevor der Decoder den nächsten Block liest.

2.  **Verarbeitungsschritt 2: Skalierung & Graustufen:**  
Das dekomprimierte Bild ist für das Display zu groß und farbig. Wir müssen es auf die Größe des Zieldisplays (128 x 64) herunterskalieren und die Farbinformationen in einen Graustufenwertumwandeln. 

3.  **Verarbeitungsschritt 3: Dithering (Monochrom-Umwandlung):**  
Jetzt haben wir ein kleines Graustufenbild. Da unser Display aber nur "An" (weiß) oder "Aus" (schwarz) kennt, müssen wir entscheiden, welcher Grauwert Schwarz und welcher weiß wird. Ein einfacher Schwellenwert würde zu hässlichen, detailarmen Bildern führen. Der **Dithering-Algorithmus von Floyd-Steinberg** ist hier die Lösung. Er macht folgendes:

    1.  Er nimmt den Grauwert des aktuellen Pixels (z.B. 100).
    2.  Er entscheidet: "Das ist näher an schwarz (0) als an weiß (255)". Er setzt das Pixel also auf Schwarz.
    3.  Er berechnet den "Fehler", den er gemacht hat: `100 - 0 = 100`.
    4.  Diesen Fehler verteilt er anteilig auf die benachbarten Pixel, die noch nicht verarbeitet wurden. Das nächste Pixel, das vielleicht einen Grauwert von 150 hatte, wird dadurch auf z.B. 180 angehoben und hat eine höhere Chance, weiß zu werden.  
    
    Ergebnis: Es entsteht eine Punktwolke, die für das menschliche Auge die Illusion von Grautönen erzeugt. Die Bildqualität ist um Welten besser.

4.  **Verarbeitungsschritt 4: XBM-Formatierung:**  
Das Ergebnis aus Schritt 3 muss nun in das XBM-Format (X BitMap) überführt werden (welches die U8g2-Bibliothek versteht). Das XBM hat diese Eigenschaften:
    - Monochrom: Es speichert nur 1-Bit-Informationen (schwarz/weiß).
    - C-Quellcode: Es ist kein binäres Dateiformat, sondern wird direkt als `C`-Quellcode gespeichert (`static const unsigned char ...`).
    - Feste Struktur: Jedes Byte repräsentiert 8 horizontale Pixel. Die Bits werden "rückwärts" gelesen, d.h. das niedrigstwertige Bit (LSB) ist das linkeste Pixel.

Das Ende der Pipeline ist ein XBM-Datenstrom, der von der `OLEDDisplaySH1106`-Klasse entgegengenommen und als Bild auf auf dem Bildschirm angezeigt werden kann.

### Proof Of Concept

Die Sketche hierzu sind unter `sketches/JPGtoXBM` abgelegt.

1.  **Proof Of Concept 1:** Wie wird ein im XBM-Format umgewandeltes JPG-Bild auf dem Display angezeigt? Ist die Qualität ausreichend? 
    *  Als Bild habe ich mein Portrait gewählt, es mit GIMP auf 128x64 Pixel verkleinert und im JPG-Format gespeichert:  
    ![JPG-Bild](./assets/frank_128x64.jpg)  
    Das JPG-Bild habe ich dann mit dem Tool [Online Image Converter to XBM](https://www.online-utility.org/image/convert/to/XBM) konvertiert:  
    ![XBM-Bild](./assets/frank_128x64.XBM.png)  
    Die resultierenden XBM-Daten habe ich als Headerdatei unter `frank_128x64_xbm.h` abgelegt. Einmal im `Include`-Verzeichnis (da ich sie auch als Splash-Screen verwenden möchte) und einmal in Ordner, in dem auch der Sketch liegt (damit die Arduino IDE diese einbinden kann).
    *  Um das XBM-Bild anzuzeigen, habe ich den Demo-Sketch `OLED_Display_SSH1106_Test.ino` genommen und alles bis auf das absolut Notwendigste entfernt. 

2.  **Proof Of Concept 2:** Wie kann auf dem ESP32 ein JPG-Bild in ein XBM-Bild umgewandelt werden? Reicht der RAM und wie lange dauert die  Verarbeitungszeit?  

    * In der Arduino IDE habe ich mittels Library Manager `https://github.com/bitbank2/JPEGDecoder` installiert.
    (TODO: nicht diese: `JPEGDEC 1.8.4 by Larry Bank`)
    * Das JPG-Bild aus PoC 1 (./assets/frank_128x64.jpg) habe ich nach folgender Anleitung von Meta-Angaben befreit:  
        1.  Öffne das Bild in GIMP.
        2.  Gehe zu `Datei -> Exportieren als...`.
        3.  Wähle als Dateityp `JPEG-Bild (*.jpg)` und gib ihm einen neuen Namen, z.B. `frank_clean_128x64.jpg` (im selben Ordner in dem auch der Sketch liegt).
        4.  Klicke auf `Exportieren`. Es öffnet sich ein Dialogfenster "Bild als JPEG exportieren".
        5.  Klappe den Bereich `Erweiterte Optionen` auf.
        6.  Entfernte die Haken bei folgenden Optionen:
            *   `EXIF-Daten speichern`
            *   `XMP-Daten speichern`
            *   `IPTC-Daten speichern`
            *   `Vorschaubild speichern`
            *   `Progressiv`
        7.  Klicke auf `Exportieren`.
    * Die Bytes des bereinigten JPG-Bildes habe ich dann in einem C-Array als Headerdatei `image_jpg.h` gespeichert (im selben Ordner wie der Sketch). Das hat das Python-Skript `convert.py` (auch im selben Ordner wie der Sketch) für mich erledigt. Statt dessen kann man auch das Online-Tool [File to hexadecimal converter](http://tomeko.net/online_tools/file_to_hex.php) nutzen - das Ergebnis ist das gleiche.

    🚧🚧🚧 TODO: Ab hier ist der PoC noch nicht umgesetzt! 🚧🚧🚧   
    Der Sketch baut auf dem Sketch von PoC 1 auf. Es lädt aber nicht die XBM-Daten, sondern die JPG-Daten, führt die Konvertierung durch und zeigt schließlich die resultierenden XBM-Daten wie im PoC 1 auf dem Display an.

3.  **Proof Of Concept 3:** Wie können die JPG-Daten von der Kamera bzw. von der SD-Karte als Datenstrom entgegengenommen werden? WIe kann der Datenstrom direkt an das Display weitergeleitet werden?

    Hier abstrahieren wir die Funktion, sodass sie nicht mehr von einer festen Datei abhängt, sondern jeden beliebigen Datenstrom verarbeiten kann.
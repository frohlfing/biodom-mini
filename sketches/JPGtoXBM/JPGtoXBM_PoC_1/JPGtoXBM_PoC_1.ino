/**
 * PoC 1: Anzeige eines XBM-Bildes auf dem OLED.
 * 
 * Details hierzu `siehe /docs/JPGtoXBM.md`.
 */

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Display-Initialisierung (aus Sketch OLED_Display_SSH1106_Test.ino übernommen)
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Das Testbild im XBM-Format ---
#include "frank_128x64_xbm.h" // definiert das C-Array frank_128x64_xbm[]

void setup() {
    Serial.begin(115200);
    Serial.println("JPGtoXBM - PoC Schritt 1: Konvertiertes JPG anzeigen");

    // Display initialisieren
    u8g2.begin();
    
    // Puffer löschen, bevor wir zeichnen
    u8g2.clearBuffer();

    // Das XBM-Format verwendet '1' für schwarze Pixel. U8g2 zeichnet '1' standardmäßig als weiße Pixel.
    // Um das zu korrigieren, erstellen wir zuerst eine weiße Leinwand und "stanzen" dann die schwarzen Pixel aus.
    u8g2.setDrawColor(1);        // 1. Zeichenfarbe auf weiß (Pixel AN) setzen.
    u8g2.drawBox(0, 0, 128, 64); // 2. Eine gefüllte Box über den gesamten Bildschirm zeichnen, um den Hintergrund weiß zu machen.
    u8g2.setDrawColor(0);        // 3. Zeichenfarbe auf schwarz (Pixel AUS) setzen. Jede '1' in den Daten "löscht" nun einen Pixel und macht ihn schwarz.

    // --- Der Kern dieses Tests ---
    // Zeichne das konvertierte Portrait in den Puffer.
    // Da das Bild bereits 128x64 Pixel groß ist, zeichnen ich es an der Ecke (0,0).
    u8g2.drawXBMP(0, 0, 128, 64, frank_128x64_xbm);
    
    // Den Puffer-Inhalt an das Display senden, um ihn sichtbar zu machen
    u8g2.sendBuffer();

    Serial.println("Konvertiertes Portrait sollte jetzt auf dem Display sichtbar sein.");
}

void loop() {
    // Nichts zu tun hier, das Bild wird einmalig im setup() angezeigt.
    delay(1000);
}
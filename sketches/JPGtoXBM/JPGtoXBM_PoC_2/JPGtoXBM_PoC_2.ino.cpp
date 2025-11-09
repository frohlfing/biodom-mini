/**
 * Proof-of-Concept und Debugging-Sketch für die JPG-zu-XBM-Konvertierung.
 * 
 * -- ZWECK DIESES SKETCHES --
 * Dieser Sketch ist ein umfassendes Werkzeug, um die On-Device-Konvertierung von
 * JPG-Bildern in monochrome Bitmaps (XBM-Format) für ein U8g2-Display zu testen und zu validieren.
 * Er dient als definitive Grundlage für die spätere Entwicklung einer "JPGtoXBM"-Bibliothek.
 * 
 * -- FUNKTIONSWEISE --
 * Der Sketch wird über zwei #define-Schalter gesteuert, die es ermöglichen, verschiedene
 * Teile der Verarbeitungs-Pipeline gezielt zu testen:
 * 
 * 1. USE_TEST_PATTERN:
 *    - true:  Es wird ein künstliches Testbild (ein einfacher horizontaler Strich) anstelle
 *             des echten JPGs verarbeitet. Dies dient dazu, die Korrektheit der Konvertierungsalgorithmen 
 *             (Thresholding/Dithering) und der Display-Ausgabe zu überprüfen.
 *    - false: Es wird das echte JPG-Bild aus der 'image_jpg.h'-Datei verarbeitet.
 * 
 * 2. USE_DITHERING:
 *    - true:  Verwendet den hochwertigen Floyd-Steinberg-Dithering-Algorithmus, um
 *             Graustufen zu simulieren. Das Ergebnis sollte dem des Online-Tools ähneln.
 *    - false: Verwendet eine simple Schwellenwert-Methode (Thresholding). Das Bild wird
 *             sehr kontrastreich und detailarm sein, dient aber als grundlegender Funktionstest.
 * 
 *  3. USE_NEGATIVE_DISPLAY:
 *    - true:  Zeigt das Ergebnis als Negativ auf dem OLED-Display an (weiße Pixel werden schwarz dargestellt).
 *    - false: Zeigt das Ergebnis normal an (weiße Pixel bleiben weiß).
 *   
 * -- ABLAUF --
 * 1. Im setup() wird die ausgewählte Bildquelle (Test-Strich oder JPG) mit dem
 *    ausgewählten Algorithmus (Dithering oder Thresholding) verarbeitet.
 * 2. Das Ergebnis wird in einem Puffer (`xbm_buffer`) im XBM-Format gespeichert.
 * 3. Der Inhalt dieses Puffers wird formatiert auf dem Seriellen Monitor ausgegeben.
 *    Dies ermöglicht die externe Verifizierung der Daten mit einem Tool wie rawpixels.net.
 * 4. Anschließend wird der Puffer-Inhalt auf dem OLED-Display angezeigt.
 * 
 * Durch den Vergleich der Ergebnisse der verschiedenen Modi kann der genaue
 * Ort eines Fehlers systematisch eingekreist werden.
 */

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <TJpg_Decoder.h>

// =======================================================================================
// === STEUERUNGS-SCHALTER ===============================================================
// =======================================================================================
// Schalte zwischen den Testmodi um, indem du 'true' oder 'false' änderst.

// Wähle die Bildquelle
// true = Simpler horizontaler Strich | false = Echtes JPG aus 'image_jpg.h'
#define USE_TEST_PATTERN false

// Wähle den Konvertierungs-Algorithmus
// true = Floyd-Steinberg Dithering | false = Simples Thresholding
#define USE_DITHERING false

// Wähle den Anzeigemodus auf dem OLED-Display
// true = Normal-Anzeige | false = Negativ-Anzeige
#define USE_NEGATIVE_DISPLAY true // Negativ-Anzeige auf OLED
// =======================================================================================

// --- Display-Initialisierung ---
// Ich verwende die bekannte Konfiguration für das SH1106 I2C Display.
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// --- Eingabedaten: Das "saubere" JPG als C-Array ---
// Erforderlich, wenn USE_TEST_PATTERN auf 'false' gesetzt ist.
// Dieses Array MUSS mit einem Tool wie dem bereitgestellten Python-Skript oder
// http://tomeko.net/online_tools/file_to_hex.php erstellt werden und darf
// keine unnötigen Metadaten enthalten.
#if !USE_TEST_PATTERN
  #include "frank_128x64_clean_jpg.h"
#endif

// --- Ausgabepuffer ---
// Hier speichern wir das konvertierte, monochrome Bild im XBM-Format.
#define XBM_WIDTH 128
#define XBM_HEIGHT 64
// 1 Bit pro Pixel -> (128 * 64) / 8 = 1024 Bytes
uint8_t xbm_buffer[XBM_WIDTH * XBM_HEIGHT / 8];

// --- TJpgDec Konfiguration & Dithering-Puffer ---
// Struktur, um Daten zwischen den Callback-Funktionen zu übergeben
struct JpegConverter {
    const uint8_t* jpg_data; // Pointer auf den Anfang der JPG-Daten
    uint32_t jpg_index;      // Aktuelle Leseposition im JPG-Array
    uint32_t jpg_size;       // Gesamtgröße des JPG-Arrays
    uint8_t* xbm_buffer;     // Pointer auf unseren Ausgabe-Puffer
};
JpegConverter conv; // Globale Instanz unserer Konverter-Struktur

// Puffer für die Fehlerverteilung beim Dithering. Muss etwas breiter als das Bild sein.
int16_t error_buffer_line1[XBM_WIDTH + 2];
int16_t error_buffer_line2[XBM_WIDTH + 2];

/**
 * @brief TJpgDec Input Callback: Liefert die JPG-Daten an den Decoder.
 *  
 * Diese Funktion wird wiederholt vom TJpgDec-Decoder aufgerufen.
 * Sie liest bis zu 'ndata' Bytes aus unserem JPG-Array und gibt sie an den
 * Decoder zurück.
 * 
 * @param buff Pointer auf einen Puffer, in den die Daten kopiert werden sollen.
 * @param ndata Die maximale Anzahl an Bytes, die gelesen werden sollen.
 * @return Die Anzahl der tatsächlich gelesenen Bytes.
 */
size_t jpg_input_callback(JDEC* jdec, uint8_t* buff, size_t ndata) {
    JpegConverter* c = (JpegConverter*)jdec->device; // Zugriff auf unsere Struktur
    
    // Prüfen, ob das Ende der Daten erreicht ist
    if (c->jpg_index >= c->jpg_size) {
        return 0;
    }
    // Bestimmen, wie viele Bytes wir tatsächlich lesen können
    size_t bytes_to_read = min(ndata, (size_t)(c->jpg_size - c->jpg_index));
    if (buff) {
        memcpy(buff, c->jpg_data + c->jpg_index, bytes_to_read);
    }

    // Leseposition aktualisieren
    c->jpg_index += bytes_to_read;

    return bytes_to_read;
}

/**
 * @brief TJpgDec Output Callback: Verarbeitet die dekomprimierten Pixel.
 *  *
 * Diese Funktion wird aufgerufen, nachdem der Decoder einen kleinen Block
 * des Bildes dekomprimiert hat. Hier findet unsere Konvertierung statt.
 * 
 * @param jdec Pointer auf die JDEC-Struktur.
 * @param bitmap Pointer auf die dekomprimierten Pixeldaten (im RGB565-Format).
 * @param rect Rechteck, das die Position und Größe des Blocks beschreibt.
 * @return 1, um die Dekompression fortzusetzen.
 */
int jpg_output_callback(JDEC* jdec, void* bitmap, JRECT* rect) {
    uint16_t* p = (uint16_t*)bitmap; // Pixel-Daten sind im RGB565-Format (16 Bit)

    for (int y = rect->top; y <= rect->bottom; y++) {
        int16_t* current_error_line = (y % 2 == 0) ? error_buffer_line1 : error_buffer_line2;
        int16_t* next_error_line    = (y % 2 == 0) ? error_buffer_line2 : error_buffer_line1;

        // Die nächste Fehlerzeile für den Start der aktuellen Zeile vorbereiten/zurücksetzen
        if (rect->left == 0) {
            memset(next_error_line, 0, sizeof(error_buffer_line1));
        }

        for (int x = rect->left; x <= rect->right; x++) {
            uint8_t original_grayscale;

            if (USE_TEST_PATTERN) {
                original_grayscale = (y == 32) ? 0 : 255;
            } else {
                // Pixel aus dem dekomprimierten Block lesen
                uint16_t color = *p++;

                // In 8-Bit Graustufen umrechnen
			    // Simple, schnelle Konvertierung aus RGB565
                uint8_t r = (color >> 11) & 0x1F;
                uint8_t g = (color >> 5) & 0x3F;
                uint8_t b = (color >> 0) & 0x1F;

                // Gewichtete Formel zur Graustufen-Umrechnung (skaliert für 5/6/5 Bit)
                original_grayscale = (r * 21 + g * 42 + b * 11) >> 2;
            }

            // In 1-Bit (schwarz/weiß) umwandeln
            int16_t new_value;
            #if USE_DITHERING
                // Floyd-Steinberg Dithering 
                int16_t corrected_gray = original_grayscale + current_error_line[x + 1];
                corrected_gray = max((int16_t)0, min((int16_t)255, corrected_gray));
                new_value = (corrected_gray < 128) ? 0 : 255;
                int16_t error = corrected_gray - new_value;
                current_error_line[x + 2] += (error * 7) / 16;
                next_error_line[x]       += (error * 3) / 16;
                next_error_line[x + 1]   += (error * 5) / 16;
                next_error_line[x + 2]   += (error * 1) / 16;
            #else 
                // Simples Thresholding
                new_value = (original_grayscale < 128) ? 0 : 255;
            #endif
            
            // Setze das Bit im XBM-Puffer
            int byte_index = (y * XBM_WIDTH + x) / 8;
            int bit_index = x % 8;

            if (new_value == 255) { // Pixel ist WEISS
                // Lasse das Bit auf 0 (bleibt weißer Hintergrund)
                conv.xbm_buffer[byte_index] |= (1 << bit_index);
            } else { // Pixel ist SCHWARZ
                // Setze das Bit auf 1 (um es später schwarz zu "stanzen")
                conv.xbm_buffer[byte_index] &= ~(1 << bit_index);
            }
        }
    }
    return 1;
}

/**
 * @brief Hilfsfunktion zur formatierten Ausgabe des XBM-Puffers auf der seriellen Schnittstelle.
 */
void print_xbm_buffer() {
    Serial.println("\n--- START XBM C-ARRAY (zur Verifizierung in rawpixels.net) ---");
    for (int i = 0; i < sizeof(xbm_buffer); i++) {
        if (xbm_buffer[i] < 0x10) Serial.print("0");
        Serial.printf("%X", xbm_buffer[i]);
    }
    Serial.println("\n--- END XBM C-ARRAY ---\n");
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("JPGtoXBM - Debugging PoC");
    
    u8g2.begin();

    // --- 1. Vorbereitung ---
    conv.xbm_buffer = xbm_buffer;
    memset(xbm_buffer, 0, sizeof(xbm_buffer));
    memset(error_buffer_line1, 0, sizeof(error_buffer_line1));
    memset(error_buffer_line2, 0, sizeof(error_buffer_line2));

    JDEC jdec;
    uint8_t workspace[4096];
    JRESULT res = JDR_OK;

    // --- 2. Konvertierung starten ---
    #if USE_TEST_PATTERN
        Serial.println("Modus: TEST-MUSTER (horizontaler Strich)");
        // Wir MÜSSEN den blockweisen Aufruf von TJpgDec simulieren,
        // da der Dithering-Algorithmus davon abhängt.
        JRECT rect;
        rect.left = 0;
        rect.right = XBM_WIDTH - 1;
        // Simuliere den Aufruf in 16-Zeilen-Blöcken
        for (int top = 0; top < XBM_HEIGHT; top += 16) {
            rect.top = top;
            rect.bottom = min(top + 15, XBM_HEIGHT - 1);
            jpg_output_callback(&jdec, nullptr, &rect);
        }
        Serial.println("Test-Muster-Konvertierung abgeschlossen.");
        res = JDR_OK;
    #else
        Serial.println("Modus: ECHTES JPG BILD");
        conv.jpg_data = image_jpg;
        conv.jpg_index = 0;
        conv.jpg_size = sizeof(image_jpg);

        res = jd_prepare(&jdec, jpg_input_callback, workspace, sizeof(workspace), &conv);
        if (res == JDR_OK) {
            uint8_t scale = 0; // Keine Skalierung, JPG ist bereits 128x64
            Serial.printf("JPG-Header OK. Größe: %dx%d.\n", jdec.width, jdec.height);
            res = jd_decomp(&jdec, jpg_output_callback, scale);
        }
    #endif

    // --- 3. Ergebnis auswerten und anzeigen ---
    if (res == JDR_OK) {
        Serial.println("Konvertierung war erfolgreich.");
        print_xbm_buffer();
        
        Serial.println("Zeige Ergebnis auf dem Display an...");
        
        // Puffer löschen, bevor wir zeichnen
        u8g2.clearBuffer();

#if USE_NEGATIVE_DISPLAY
        // Das XBM-Format verwendet '1' für schwarze Pixel. U8g2 zeichnet '1' standardmäßig als weiße Pixel.
        // Um das zu korrigieren, erstellen wir zuerst eine weiße Leinwand und "stanzen" dann die schwarzen Pixel aus.
        u8g2.setDrawColor(1);        // 1. Zeichenfarbe auf weiß (Pixel AN) setzen.
        u8g2.drawBox(0, 0, 128, 64); // 2. Eine gefüllte Box über den gesamten Bildschirm zeichnen, um den Hintergrund weiß zu machen.
        u8g2.setDrawColor(0);        // 3. Zeichenfarbe auf schwarz (Pixel AUS) setzen. Jede '1' in den Daten "löscht" nun einen Pixel und macht ihn schwarz.
#endif

        // Zeichne das konvertierte Bild in den Puffer.
        // Da das Bild bereits 128x64 Pixel groß ist, zeichnen ich es an der Ecke (0,0).
        u8g2.drawXBMP(0, 0, XBM_WIDTH, XBM_HEIGHT, xbm_buffer);
        
        // Den Puffer-Inhalt an das Display senden, um ihn sichtbar zu machen
        u8g2.sendBuffer();

        Serial.println("Fertig.");

    } else {
        Serial.printf("Ein Fehler ist aufgetreten! Code: %d\n", res);
    }
}

void loop() {}
#pragma once

#include <Arduino.h>
#include <Stream.h>

// Im Original-Sketch sollte man die ArduCAM-Bibliothek für die Hardware anpassen, indem man in memorysaver.h das Kameramodell einkommentiert. 
// Das ist uncool, ich definiere hier direkt die Hardware und lasse die Hersteller-Bibliothek unangetastet.
//#include "memorysaver.h" // nicht aufrufen, hat ja nicht die richtige Kamera definiert
#define _MEMORYSAVER_ // Verhindert, dass memorysaver.h die Kamera-Konfiguration ändert. NOLINT(*-reserved-identifier)
#define OV2640_MINI_2MP_PLUS // Definiert das Kameramodell als OV2640 Mini 2MP Plus
#include <ArduCAM.h>  // Die Hauptbibliothek für die Steuerung der ArduCAM-Hardware.

/**
 * Eine Klasse zur Ansteuerung der ArduCAM Mini 2MP Plus (mit OV2640-Chip).
 */
class ArduCamMini2MPPlusOV2640
{
public:
    /**
     * @enum ImageSize
     * @brief Definiert die verfügbaren JPEG-Auflösungen des OV2640-Sensors.
     */
    enum ImageSize {
        S_160x120,      // Geringste Auflösung, kleinste Dateigröße
        S_176x144,      // QSIF-Auflösung
        S_320x240,      // QVGA-Auflösung
        S_352x288,      // CIF-Auflösung
        S_640x480,      // VGA-Auflösung
        S_800x600,      // SVGA-Auflösung
        S_1024x768,     // XGA-Auflösung
        S_1280x1024,    // SXGA-Auflösung
        S_1600x1200     // UXGA-Auflösung, maximale Auflösung des Sensors
    };
    
    /**
     * @enum LightMode
     * @brief Definiert die Weißabgleich-Modi der Kamera.
     */
    enum LightMode {
        LM_AUTO,        // Automatischer Weißabgleich
        LM_SUNNY,       // Für sonnige Umgebungen (Tageslicht)
        LM_CLOUDY,      // Für bewölkte Umgebungen
        LM_OFFICE,      // Für Beleuchtung durch Leuchtstoffröhren
        LM_HOME         // Für Beleuchtung durch Glühlampen
    };


    /**
     * @enum ColorSaturation
     * @brief Definiert die Stufen für die Farbsättigung.
     */
    enum ColorSaturation {
        SAT_PLUS_2,     // Starke Anhebung der Sättigung (+2)
        SAT_PLUS_1,     // Leichte Anhebung der Sättigung (+1)
        SAT_DEFAULT,    // Normale Sättigung (0)
        SAT_MINUS_1,    // Leichte Absenkung der Sättigung (-1)
        SAT_MINUS_2     // Starke Absenkung der Sättigung (-2)
    };

    /**
     * @enum Brightness
     * @brief Definiert die Stufen für die Helligkeit.
     */
    enum Brightness {
        BRIGHT_PLUS_2,  // Starke Anhebung der Helligkeit (+2)
        BRIGHT_PLUS_1,  // Leichte Anhebung der Helligkeit (+1)
        BRIGHT_DEFAULT, // Normale Helligkeit (0)
        BRIGHT_MINUS_1, // Leichte Absenkung der Helligkeit (-1)
        BRIGHT_MINUS_2  // Starke Absenkung der Helligkeit (-2)
    };

    /**
     * @enum Contrast
     * @brief Definiert die Stufen für den Kontrast.
     */
    enum Contrast {
        CON_PLUS_2,     // Starke Anhebung des Kontrasts (+2)
        CON_PLUS_1,     // Leichte Anhebung des Kontrasts (+1)
        CON_DEFAULT,    // Normaler Kontrast (0)
        CON_MINUS_1,    // Leichte Absenkung des Kontrasts (-1)
        CON_MINUS_2     // Starke Absenkung des Kontrasts (-2)
    };

    /**
     * @enum SpecialEffect
     * @brief Definiert die verfügbaren Spezialeffekte.
     * @note Die Namen sind mit dem Prefix 'EFFECT_' versehen, um Namenskonflikte mit der ArduCAM.h (z.B. BW) zu vermeiden.
     */
    enum SpecialEffect {
        EFFECT_ANTIQUE,     // Sepia/Antik-Effekt
        EFFECT_BLUISH,      // Blaustich-Effekt
        EFFECT_GREENISH,    // Grünstich-Effekt
        EFFECT_REDDISH,     // Rotstich-Effekt
        EFFECT_BW,          // Schwarz-Weiß-Effekt
        EFFECT_NEGATIVE,    // Negativ-Effekt (Farben invertiert)
        EFFECT_BW_NEGATIVE, // Schwarz-Weiß-Negativ-Effekt
        EFFECT_NORMAL       // Normaler Modus ohne Effekt
    };

    /**
     * @brief Konstruktor der Kamera-Klasse.
     * @param csPin Der GPIO-Pin, der als Chip Select für die ArduCAM verwendet wird.
     */
    explicit ArduCamMini2MPPlusOV2640(uint8_t csPin);

    /**
     * @brief Initialisiert die Kamera-Hardware und die SPI/I2C-Kommunikation.
     * Muss im setup() des Hauptprogramms aufgerufen werden.
     * @return true bei erfolgreicher Initialisierung, andernfalls false.
     */
    bool begin();

    /**
     * @brief Löst eine Aufnahme aus und streamt das JPEG-Bild direkt an ein Ziel.
     * @param size Die gewünschte Bildauflösung.
     * @param destination Eine Referenz auf ein Stream-Objekt (z.B. Serial oder eine Datei).
     * @return true bei Erfolg, andernfalls false.
     */
    bool captureAndStreamTo(ImageSize size, Stream &destination);

    // --- Methoden zur Konfiguration der Kamera ---

    /**
     * @brief Stellt den Weißabgleich-Modus der Kamera ein.
     * @param mode Der gewünschte Lichtmodus (z.B. `LM_AUTO`, `LM_SUNNY`).
     */
    void setLightMode(LightMode mode);

    /**
     * @brief Passt die Farbsättigung des Bildes an.
     * @param level Die gewünschte Sättigungsstufe (z.B. `SAT_PLUS_1`, `SAT_DEFAULT`).
     */
    void setColorSaturation(ColorSaturation level);

    /**
     * @brief Passt die Helligkeit des Bildes an.
     * @param level Die gewünschte Helligkeitsstufe (z.B. `BRIGHT_MINUS_1`, `BRIGHT_DEFAULT`).
     */
    void setBrightness(Brightness level);

    /**
     * @brief Passt den Kontrast des Bildes an.
     * @param level Die gewünschte Kontraststufe (z.B. `CON_PLUS_2`, `CON_DEFAULT`).
     */
    void setContrast(Contrast level);

    /**
     * @brief Wendet einen visuellen Spezialeffekt auf das Bild an.
     * @param effect Der gewünschte Spezialeffekt (z.B. `EFFECT_BW`, `EFFECT_SEPIA`).
     */
    void setSpecialEffect(SpecialEffect effect);

private:
    uint8_t _csPin;      // Der GPIO-Pin für den Chip Select der Kamera.
    ArduCAM _myCAM;      // Die Instanz der originalen ArduCAM-Treiberbibliothek.
    
    /**
     * @brief Interne Hilfsfunktion, die den FIFO-Puffer ausliest und in den Stream schreibt.
     * @param destination Der Ziel-Stream.
     * @return true bei Erfolg, andernfalls false.
     */
    bool readFifoToStream(Stream &destination);
};
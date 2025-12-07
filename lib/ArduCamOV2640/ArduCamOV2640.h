#pragma once

#include <Arduino.h>

// Im Original-Sketch sollte man die ArduCAM-Bibliothek für die Hardware anpassen, indem man in memorysaver.h das
// Kameramodell einkommentiert. Uncool! Ich definiere hier direkt die Hardware und lasse die Hersteller-Bibliothek
// unangetastet.
//#include "memorysaver.h" // nicht aufrufen, hat ja nicht die richtige Kamera definiert
#define _MEMORYSAVER_ // Verhindert, dass memorysaver.h die Kamera-Konfiguration ändert. NOLINT(*-reserved-identifier)
#define OV2640_MINI_2MP_PLUS // Definiert das Kameramodell als OV2640 Mini 2MP Plus
#include <ArduCAM.h>  // Hauptbibliothek für die Steuerung der ArduCAM-Hardware

// Bugfix "für error: macro "swap" requires 3 arguments, but only 1 given"
// Die ArduCAM.h definiert ein inkompatibles "swap"-Makro. Wir entfernen (undefine) es sofort wieder, um Konflikte mit
// den C++ Standardbibliotheken (wie <memory> oder <algorithm>) zu verhindern.
#ifdef swap
  #undef swap
#endif

/**
 * Eine Klasse zur Ansteuerung der ArduCAM OV2640 Mini 2MP Plus.
 */
class ArduCamOV2640
{
public:
    /**
     * @brief Konstruktor der Kamera-Klasse.
     * @param csPin Der GPIO-Pin, der als Chip Select für die ArduCAM verwendet wird.
     */
    explicit ArduCamOV2640(uint8_t csPin);

    /**
     * @brief Initialisiert die Kamera-Hardware und die SPI/I2C-Kommunikation.
     * Muss im setup() des Hauptprogramms aufgerufen werden.
     * @return true bei erfolgreicher Initialisierung, andernfalls false.
     */
    bool begin();

    /**
     * @brief Ändert die JPEG-Auflösung.
     * @param resolution Auflösung:
     *  OV2640_160x120 (QVGA)
     *  OV2640_176x144 (QCIF)
     *  OV2640_320x240 (QVGA, default)
     *  OV2640_352x288 (CIF)
     *  OV2640_640x480 (VGA)
     *  OV2640_800x600 (SVGA)
     *  OV2640_1024x768 (XGA)
     *  OV2640_1280x1024 (SXGA)
     *  OV2640_1600x1200 (UXGA)
     */
    void setResolution(uint8_t resolution);

    /**
     * @brief Setzt den Weißabgleich-Modus.
     * @param mode Modus:
     *  Auto (automatisch, default)
     *  Sunny (sonnig, Gelb-Stich entfernen)
     *  Cloudy (wolkig)
     *  Office (Büro, Leuchtstoffröhren/kalt)
     *  Home (Zuhause, Glühbirnen/warm)
     */
    void setLightMode(uint8_t mode);

    /**
     * @brief Setzt die Farbsättigung.
     * @param saturation Farbsättigung:
     *  Saturation_2 (fast schwarzweiß)
     *  Saturation_1 (blass)
     *  Saturation0 (normal, default)
     *  Saturation1 (kräftig)
     *  Saturation2 (hoch)
     */
    void setColorSaturation(uint8_t saturation);

    /**
     * @brief Setzt die Helligkeit.
     * @param brightness Helligkeit:
     *   Brightness_2 (-2, sehr dunkel)
     *   Brightness_1 (-1, dunkel)
     *   Brightness0 (normal, default)
     *   Brightness1 (+1, hell)
     *   Brightness2 (+2, sehr hell)
     */
    void setBrightness(uint8_t brightness);

    /**
     * @brief Setzt den Kontrast.
     * @param contrast Helligkeit:
     *  Contrast_2 (-2, flau, grau in grau)
     *  Contrast_1 (-1)
     *  Contrast0 (normal, default)
     *  Contrast1 (+1)
     *  Contrast2 (+2, stark, harte Kanten)
     */
    void setContrast(uint8_t contrast);

    /**
     * @brief Aktiviert Spezialeffekte (Filter)
     * @param effect Spezialeffekt:
     *  Antique (Altmodisch)
     *  Bluish (Blaustich)
     *  Greenish (Grünstich)
     *  Reddish (Rotstich)
     *  BW (Schwarzweiß)
     *  Negative (Farben invertiert)
     *  BWnegative (Schwarzweiß Negativ)
     *  Normal  (kein Effekte, default)
     */
    void setSpecialEffect(uint8_t effect);

    /**
     * @brief Nimmt ein Bild auf und speichert es auf der SD-Karte.
     * @param filename Dateiname (z.B. "/bild.jpg")
     * @return true bei Erfolg, andernfalls false.
     */
    bool saveToSD(const char *filename);

    /**
     * @brief Nimmt ein Bild auf und sendet das Bild inklusiv Protokoll-Marker (FF AA / FF BB) über Serial.
     * @return true bei Erfolg.
     */
    bool sendToSerialHost();

    /**
     * @brief Gibt den letzten Fehlercode zurück.
     * @return Fehlercode gemäß SimpleDHT-Definition.
     */
    int getLastError() const;

    /**
     * @brief Gibt eine Beschreibung des letzten Fehlers zurück.
     * @return Fehlerbeschreibung (max. 21 Zeichen).
     */
    const char* getErrorMessage() const;

private:
    uint8_t _csPin; // Der GPIO-Pin für den Chip Select der Kamera.
    ArduCAM _myCAM; // Die Instanz der originalen ArduCAM-Treiberbibliothek.
    int _lastError; // Fehlercode

    /**
     * @brief Schießt ein Foto und speichert die Daten in den FIFO-Puffer.
     */
    void takePicture();

    /**
     * @brief List die Daten aus dem FIFO-Puffer in den Stream.
     * @param stream Der Ziel-Stream.
     * @return true bei Erfolg, andernfalls false.
     */
    bool writeFifoToStream(Stream &stream);
};
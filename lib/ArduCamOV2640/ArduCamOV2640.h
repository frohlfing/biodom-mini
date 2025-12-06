#pragma once

#include <Arduino.h>

// Im Original-Sketch sollte man die ArduCAM-Bibliothek für die Hardware anpassen, indem man in memorysaver.h das
// Kameramodell einkommentiert. Uncool! Ich definiere hier direkt die Hardware und lasse die Hersteller-Bibliothek
// unangetastet.
//#include "memorysaver.h" // nicht aufrufen, hat ja nicht die richtige Kamera definiert
#define _MEMORYSAVER_ // Verhindert, dass memorysaver.h die Kamera-Konfiguration ändert. NOLINT(*-reserved-identifier)
#define OV2640_MINI_2MP_PLUS // Definiert das Kameramodell als OV2640 Mini 2MP Plus
#include <ArduCAM.h>  // Hauptbibliothek für die Steuerung der ArduCAM-Hardware

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
     * @param resolution Eine der ArduCAM-Konstanten, z.B. OV2640_320x240 oder OV2640_1600x1200.
     */
    void setResolution(uint8_t resolution);

    /**
     * @brief Löst eine Aufnahme aus und speichert das Bild auf ein Dateisystem (SD).
     * @param filename Dateiname (z.B. "/bild.jpg")
     * @return true bei Erfolg, andernfalls false.
     */
    bool capture(const char *filename);

    /**
     * @brief Löst eine Aufnahme aus und sendet das Bild über Serial (für Host V2 Software).
     * @return true bei Erfolg.
     */
    bool print();

private:
    uint8_t _csPin;      // Der GPIO-Pin für den Chip Select der Kamera.
    ArduCAM _myCAM;      // Die Instanz der originalen ArduCAM-Treiberbibliothek.

    /**
     * @brief Schießt ein Foto und speichert die Daten in den FIFO-Puffer.
     */
    void captureToFifo();

    /**
     * @brief List die Daten aus dem FIFO-Puffer in den Stream.
     * @param stream Der Ziel-Stream.
     * @return true bei Erfolg, andernfalls false.
     */
    bool writeFifoToStream(Stream &stream);
};
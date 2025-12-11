#include "ArduCamOV2640.h"
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>

ArduCamOV2640::ArduCamOV2640(const uint8_t csPin)
    : _csPin(csPin), _myCAM(OV2640, csPin), _lastError(0) {}

bool ArduCamOV2640::begin() {
    // CS-Pin konfigurieren
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);

    // 1. CPLD (Chip auf der ArduCAM) zurücksetzen
    _myCAM.write_reg(0x07, 0x80);
    delay(100);
    _myCAM.write_reg(0x07, 0x00);
    delay(100);

    // 2. SPI-Verbindung zur ArduCAM testen
    _myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    if (_myCAM.read_reg(ARDUCHIP_TEST1) != 0x55) {
        _lastError = 1;  // SPI-Fehler
        return false;
    }

    // 3. Kamerasensor prüfen
    uint8_t vid, pid;
    _myCAM.wrSensorReg8_8(0xff, 0x01);
    _myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    _myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if (vid != 0x26 || (pid != 0x41 && pid != 0x42)) {
        _lastError = 2; // Falscher Kamerasensor
        return false;
    }

    // 4. Kamera konfigurieren
    _myCAM.set_format(JPEG);
    _myCAM.InitCAM();
    _myCAM.OV2640_set_JPEG_size(OV2640_320x240); // QVGA

    //  Sensor Zeit geben, die neuen Einstellungen zu verarbeiten (Weißabgleich etc.)
    delay(200);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
    return true;
}

void ArduCamOV2640::setResolution(const uint8_t resolution) {
    // FIFO in einen sauberen Zustand versetzen
    _myCAM.flush_fifo();
    _myCAM.clear_fifo_flag();

    // Befehl an den Sensor senden
    _myCAM.OV2640_set_JPEG_size(resolution);

    // Warten, bis der Sensor sich stabilisiert hat
    delay(200);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
}

void ArduCamOV2640::setLightMode(const uint8_t mode) {
    _myCAM.OV2640_set_Light_Mode(mode);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
}

void ArduCamOV2640::setColorSaturation(const uint8_t saturation) {
    _myCAM.OV2640_set_Color_Saturation(saturation);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
}

void ArduCamOV2640::setBrightness(const uint8_t brightness) {
    _myCAM.OV2640_set_Brightness(brightness);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
}

void ArduCamOV2640::setContrast(const uint8_t contrast) {
    _myCAM.OV2640_set_Contrast(contrast);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
}

void ArduCamOV2640::setSpecialEffect(const uint8_t effect) {
    _myCAM.OV2640_set_Special_effects(effect);

    // CS-Pin freigeben, um SPI-Bus-Konflikte zu vermeiden
    //digitalWrite(_csPin, HIGH);
}

bool ArduCamOV2640::saveToSD(const char *filename) {
    _lastError = 0;

    // Bild aufzeichnen
    takePicture();

    // Bild speichern
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        _lastError = 3; // Failed to open file for writing
        return false;
    }
    const bool success = writeFifoToStream(file);
    file.close();
    return success;
}

bool ArduCamOV2640::sendToSerialHost() {
    _lastError = 0;

    // Bild aufzeichnen
    takePicture();

    // Protokoll-Start-Marker für Host-Software
    Serial.write(0xFF);
    Serial.write(0xAA);

    // Inhalt senden
    const bool success = writeFifoToStream(Serial);

    // Protokoll-End-Marker
    Serial.write(0xFF);
    Serial.write(0xBB);
    return success;
}

void ArduCamOV2640::takePicture() {
    // Bild aufzeichnen
    _myCAM.flush_fifo(); // Puffer leeren
    _myCAM.clear_fifo_flag(); // Status-Register zurücksetzen (bereit für die erste Aufnahme)
    _myCAM.start_capture(); // Aufnahme starten
    while (!_myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
        // warten, bis das Bild fertig im FIFO liegt
        yield(); // Wichtig für ESP32 Watchdog
    }
}

bool ArduCamOV2640::writeFifoToStream(Stream &stream)
{
    uint32_t length = _myCAM.read_fifo_length();

    // Sicherheitscheck: Wenn Länge 0 oder riesig (Fehler), abbrechen
    if (length >= MAX_FIFO_SIZE || length == 0) {
        // Fehlerfall: Wir schreiben eine kurze Fehlermeldung in den Stream,
        // falls es Serial ist, sieht man es im Terminal.
        // Auf SD würde das File leer bleiben oder Text enthalten.
        _lastError = 4; // FIFO-Länge 0 oder Max
        return false;
    }

    // Puffer für blockweises Schreiben (schneller als Byte-by-Byte)
    constexpr int bufferSize = 256;
    byte buf[bufferSize];
    int i = 0;

    uint8_t temp = 0;
    uint8_t temp_last = 0;
    bool is_header = false; // Hilfsvariable zum Finden des JPEG-Starts

    _myCAM.CS_LOW();
    _myCAM.set_fifo_burst(); // Burst-Read Modus aktivieren

    // Den gesamten FIFO-Inhalt auslesen
    bool success = false;
    while (length--) {
        temp_last = temp;
        temp = SPI.transfer(0x00);
        if (is_header) {
            // Wir befinden uns mitten in den Bilddaten oder am Ende...
            buf[i++] = temp; // Byte in den Puffer schreiben

            // Puffer voll? Dann schreiben!
            if (i >= bufferSize) {
                _myCAM.CS_HIGH(); // Kurz SPI unterbrechen (manchmal nötig für Stabilität, hier optional aber sicher)
                stream.write(buf, bufferSize); // Blockweise senden ist viel schneller als einzeln!

                // Da wir CS_HIGH sind, ist der SPI-Bus frei.
                // Das ist der perfekte Moment, um dem System kurz Luft zu geben.
                yield(); // 2. Watchdog streicheln!

                i = 0; // Puffer-Index zurücksetzen
                _myCAM.CS_LOW(); // SPI-Bus wieder für Kamera aktivieren
                _myCAM.set_fifo_burst();
            }

            if (temp == 0xD9 && temp_last == 0xFF) {
                // Ende des JPEGs (Marker 0xFF 0xD9) erkannt
                _myCAM.CS_HIGH(); // SPI-Bus für die Kamera freigeben...
                stream.write(buf, i); // ...und den Rest des Puffers auf die SD-Karte schreiben
                success = true;
                break; // Bild ist komplett
            }
        }
        else if (temp == 0xD8 && temp_last == 0xFF) {
            // Anfang des JPEGs (Marker 0xFF 0xD8) erkannt
            is_header = true;
            // Die ersten beiden Bytes des JPEGs in den Puffer schreiben
            buf[i++] = temp_last;
            buf[i++] = temp;
        }
    }

    _myCAM.CS_HIGH();

    if (!success) {
        _lastError = 5; // JPEG-Ende (0xFF,0xD9) nicht gefunden
    }
    return success;
}

int ArduCamOV2640::getLastError() const {
    return _lastError;
}

const char* ArduCamOV2640::getErrorMessage() const {
    switch(_lastError) {
        case 0: return "OK";
        case 1: return "SPI-Fehler";
        case 2: return "Falscher Kamerasensor";
        case 3: return "Schreibfehler";
        case 4: return "FIFO-Länge 0 oder Max";
        case 5: return "Kein JPEG-Ende";
        default: return "Unbekannter Fehler";
    }
}
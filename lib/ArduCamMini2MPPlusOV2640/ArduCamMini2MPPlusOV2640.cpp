#include "ArduCamMini2MPPlusOV2640.h"
#include <Wire.h>
#include <SPI.h>

ArduCamMini2MPPlusOV2640::ArduCamMini2MPPlusOV2640(const uint8_t csPin) 
    : _csPin(csPin), _myCAM(OV2640, csPin) 
{
    // Die Initializer-List hat bereits die _myCAM-Instanz korrekt initialisiert.
}

bool ArduCamMini2MPPlusOV2640::begin()
{
    // I2C-Bus für die Kamerakonfiguration initialisieren.
    #if defined(__SAM3X8E__)
        Wire1.begin();
    #else
        Wire.begin();
    #endif

    // SPI-Bus initialisieren
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    SPI.begin();

    // 1. CPLD (Hilfs-Chip auf der ArduCAM) zurücksetzen.
    _myCAM.write_reg(0x07, 0x80);
    delay(100);
   _myCAM.write_reg(0x07, 0x00);
    delay(100);

    // 2. SPI-Verbindung zur ArduCAM testen
    _myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    if (_myCAM.read_reg(ARDUCHIP_TEST1) != 0x55) {
        return false; // SPI-Fehler
    }

    // 3. Kamerasensor-Modell überprüfen
    uint8_t vid, pid;
    _myCAM.wrSensorReg8_8(0xff, 0x01);
    _myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    _myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    if (vid != 0x26 || (pid != 0x41 && pid != 0x42)) {
        return false; // Falscher Kamerasensor
    }

    // 4. Kamera konfigurieren
    _myCAM.set_format(JPEG);
    _myCAM.InitCAM();
    _myCAM.clear_fifo_flag();
    
    return true;
}

bool ArduCamMini2MPPlusOV2640::captureAndStreamTo(ImageSize size, Stream &destination)
{
    // 1. Auflösung setzen
    switch(size) {
        case S_160x120:   _myCAM.OV2640_set_JPEG_size(OV2640_160x120);   break;
        case S_176x144:   _myCAM.OV2640_set_JPEG_size(OV2640_176x144);   break;
        case S_320x240:   _myCAM.OV2640_set_JPEG_size(OV2640_320x240);   break;
        case S_352x288:   _myCAM.OV2640_set_JPEG_size(OV2640_352x288);   break;
        case S_640x480:   _myCAM.OV2640_set_JPEG_size(OV2640_640x480);   break;
        case S_800x600:   _myCAM.OV2640_set_JPEG_size(OV2640_800x600);   break;
        case S_1024x768:  _myCAM.OV2640_set_JPEG_size(OV2640_1024x768);  break;
        case S_1280x1024: _myCAM.OV2640_set_JPEG_size(OV2640_1280x1024); break;
        case S_1600x1200: _myCAM.OV2640_set_JPEG_size(OV2640_1600x1200); break;
    }
    delay(100); // Zeit geben, damit die Einstellungen übernommen werden

    // 2. Aufnahme vorbereiten und starten
    _myCAM.flush_fifo();
    _myCAM.clear_fifo_flag();
    _myCAM.start_capture();

    // 3. Warten, bis die Aufnahme abgeschlossen ist
    while (!_myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {}

    // 4. Daten aus dem FIFO lesen und in den Ziel-Stream schreiben
    bool success = readFifoToStream(destination);

    // 5. Status zurücksetzen
    _myCAM.clear_fifo_flag();

    return success;
}

void ArduCamMini2MPPlusOV2640::setLightMode(LightMode mode) {
    switch (mode) {
        case LM_AUTO:   _myCAM.OV2640_set_Light_Mode(Auto);   break;
        case LM_SUNNY:  _myCAM.OV2640_set_Light_Mode(Sunny);  break;
        case LM_CLOUDY: _myCAM.OV2640_set_Light_Mode(Cloudy); break;
        case LM_OFFICE: _myCAM.OV2640_set_Light_Mode(Office); break;
        case LM_HOME:   _myCAM.OV2640_set_Light_Mode(Home);   break;
    }
}

void ArduCamMini2MPPlusOV2640::setColorSaturation(ColorSaturation level) {
    switch (level) {
        case SAT_PLUS_2:    _myCAM.OV2640_set_Color_Saturation(Saturation2);  break;
        case SAT_PLUS_1:    _myCAM.OV2640_set_Color_Saturation(Saturation1);  break;
        case SAT_DEFAULT:   _myCAM.OV2640_set_Color_Saturation(Saturation0);  break;
        case SAT_MINUS_1:   _myCAM.OV2640_set_Color_Saturation(Saturation_1); break;
        case SAT_MINUS_2:   _myCAM.OV2640_set_Color_Saturation(Saturation_2); break;
    }
}

void ArduCamMini2MPPlusOV2640::setBrightness(Brightness level) {
    switch (level) {
        case BRIGHT_PLUS_2:    _myCAM.OV2640_set_Brightness(Brightness2);  break;
        case BRIGHT_PLUS_1:    _myCAM.OV2640_set_Brightness(Brightness1);  break;
        case BRIGHT_DEFAULT:   _myCAM.OV2640_set_Brightness(Brightness0);  break;
        case BRIGHT_MINUS_1:   _myCAM.OV2640_set_Brightness(Brightness_1); break;
        case BRIGHT_MINUS_2:   _myCAM.OV2640_set_Brightness(Brightness_2); break;
    }
}

void ArduCamMini2MPPlusOV2640::setContrast(Contrast level) {
    switch (level) {
        case CON_PLUS_2:    _myCAM.OV2640_set_Contrast(Contrast2);  break;
        case CON_PLUS_1:    _myCAM.OV2640_set_Contrast(Contrast1);  break;
        case CON_DEFAULT:   _myCAM.OV2640_set_Contrast(Contrast0);  break;
        case CON_MINUS_1:   _myCAM.OV2640_set_Contrast(Contrast_1); break;
        case CON_MINUS_2:   _myCAM.OV2640_set_Contrast(Contrast_2); break;
    }
}

void ArduCamMini2MPPlusOV2640::setSpecialEffect(SpecialEffect effect) {
    // Mappt unser sicheres Enum auf die (potenziell problematischen) Defines der Bibliothek
    switch (effect) {

        case EFFECT_ANTIQUE:     _myCAM.OV2640_set_Special_effects(Antique);     break;
        case EFFECT_BLUISH:      _myCAM.OV2640_set_Special_effects(Bluish);      break;
        case EFFECT_GREENISH:    _myCAM.OV2640_set_Special_effects(Greenish);    break;
        case EFFECT_REDDISH:     _myCAM.OV2640_set_Special_effects(Reddish);     break;
        case EFFECT_BW:          _myCAM.OV2640_set_Special_effects(BW);          break;
        case EFFECT_NEGATIVE:    _myCAM.OV2640_set_Special_effects(Negative);    break;
        case EFFECT_BW_NEGATIVE: _myCAM.OV2640_set_Special_effects(BWnegative);  break;
        case EFFECT_NORMAL:      _myCAM.OV2640_set_Special_effects(Normal);      break;
        default:
            break;
    }
}

bool ArduCamMini2MPPlusOV2640::readFifoToStream(Stream &destination)
{
    uint32_t length = _myCAM.read_fifo_length();
    if (length >= MAX_FIFO_SIZE || length == 0) {
        return false; // Fehler: Ungültige Puffergröße
    }
    
    // Wir verwenden eine Puffergröße, die ein Vielfaches der Sektorgröße von SD-Karten (512 Bytes) ist.
    // Dies kann die Schreibleistung auf SD-Karten erheblich verbessern. 4096 ist ein guter Kompromiss.
    constexpr size_t bufferSize = 4096;
    uint8_t buffer[bufferSize];
    size_t i = 0; // Index für den Puffer

    _myCAM.CS_LOW();
    _myCAM.set_fifo_burst();

    while (length--) {
        // Lese ein Byte aus dem FIFO in den Puffer
        buffer[i++] = SPI.transfer(0x00);
        
        // Wenn der Puffer voll ist, schreibe ihn in einem Rutsch in den Ziel-Stream
        if (i >= bufferSize) {
            destination.write(buffer, bufferSize);
            i = 0; // Setze den Puffer-Index für die nächste Runde zurück
        }
    }
    
    // Nach der Schleife können noch restliche Bytes im Puffer sein.
    // Schreibe diese ebenfalls in den Stream.
    if (i > 0) {
        destination.write(buffer, i);
    }

    _myCAM.CS_HIGH();
    return true;
}
/**
 * @file ArduCamMini2MPPlusOV2640.h
 * Wrapper für ArduCAM Mini 2MP Plus (OV2640) — ESP32-Fokus
 *
 * Diese Wrapper-Klasse kapselt die ArduCAM Arducam_mini Library (Upstream)
 * und integriert direkt die MicroSDCard-API (SdFat-backed) für RAM-schonende,
 * atomare Speicherung von Aufnahmen.
 *
 * Designprinzip:
 * - Schlanke, leicht verständliche API für typische Workflows (captureToSD, captureToBuffer, streaming)
 * - RAM-schonendes Chunked-IO (default 512 B)
 * - Atomare Speicherung via MicroSDCard (tmp -> rename)
 *
 * Abhängigkeiten:
 * - ArduCAM/Arducam_mini (Upstream, empfohlen per Tag v1.0.2).
 * - MicroSDCard (deine lokale SdFat-basiere MicroSDCard-Wrapper-Lib).
 *
 * Lizenz: Wrapper-Lib unter MIT; ArduCAM Upstream ist LGPL-2.1 (siehe Upstream).
 */

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// Upstream ArduCAM headers (werden per lib_deps eingebunden)
#include <ArduCAM.h>
#include <memorysaver.h>
#include <ov2640_regs.h>

// Lokale MicroSDCard-API
#include <MicroSDCard.h>

class ArduCamMini2MPPlusOV2640 {
public:
  /**
   * Konstruktor.
   * @param csPin SPI Chip Select Pin für ArduCAM (Default 5).
   * @param i2cSda I2C SDA Pin (Default 21).
   * @param i2cScl I2C SCL Pin (Default 22).
   * @param spiClockHz SPI SCLK (Default 8000000UL).
   * @param chunkSize Chunk-Größe in Bytes beim SD-Streaming (Default 512).
   * @param timeoutMs Timeout für Capture-Operationen in ms (Default 5000).
   */
  ArduCamMini2MPPlusOV2640(uint8_t csPin = 5,
                           uint8_t i2cSda = 21,
                           uint8_t i2cScl = 22,
                           uint32_t spiClockHz = 8000000UL,
                           size_t chunkSize = 512,
                           uint32_t timeoutMs = 5000);

  /**
   * Initialisiert SPI, I2C und die Kamera.
   * Muss in setup() aufgerufen werden.
   * @return true bei Erfolg, false bei Fehler (z. B. Kamera nicht gefunden).
   */
  bool begin();

  /**
   * Prüft, ob die Kamera erreichbar ist.
   * @return true wenn Kamera-Präsenz erkannt wurde.
   */
  bool isCameraPresent();

  /**
   * Setzt den JPEG-Modus / Aufnahmemodus auf JPEG.
   * @return true bei Erfolg.
   */
  bool setFormatJPEG();

  /**
   * Nimmt ein Bild auf und liefert es in einen neu allokierten Buffer.
   * WARNUNG: RAM-intensiv — nur für kleine Bilder / Tests.
   * @param outBuf Pointer-Referenz, auf die ein neu allokierter Buffer geschrieben wird.
   *               Der Aufrufer ist verantwortlich, den Buffer später zu free()en.
   * @param outLen Länge des Buffers in Bytes.
   * @param timeoutMs maximal erlaubte Wartezeit in ms.
   * @return true bei Erfolg, false bei Fehler.
   */
  bool captureToBuffer(uint8_t** outBuf, size_t &outLen, uint32_t timeoutMs = 0);

  /**
   * Nimmt ein Bild auf und speichert es direkt auf MicroSD (atomar).
   * Workflow:
   *  - Öffnet sd.streamWriteStart(path)
   *  - Liest Kamera-FIFO in chunks (this->chunkSize) und ruft sd.streamWriteChunk
   *  - Schließt mit sd.streamWriteEnd() (tmp -> final rename)
   * @param sd Referenz auf eine initialisierte MicroSDCard-Instanz.
   * @param path Zielpfad auf Karte, z. B. "/img001.jpg".
   * @param timeoutMs Timeout in ms (0 => Default aus Konstruktor).
   * @return true bei Erfolg, false bei Fehler.
   */
  bool captureToSD(MicroSDCard &sd, const char* path, uint32_t timeoutMs = 0);

  /**
   * Nimmt ein Bild auf und liefert die Daten stückweise via Callback.
   * Callback wird mit (buf,len,user) aufgerufen; wenn callback false zurückgibt,
   * wird das Lesen abgebrochen.
   * @param cb Callback-Funktion, die jeweils ein Chunk verarbeitet.
   * @param user Optionaler User-Pointer, wird an cb übergeben.
   * @param timeoutMs Timeout in ms (0 => Default).
   * @return true bei Erfolg, false bei Fehler.
   */
  bool captureStreaming(bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user = nullptr, uint32_t timeoutMs = 0);

  /**
   * Soft-Reset der Kamera (wenn unterstützt).
   */
  void softReset();

  /**
   * Setzt die Chunk-Größe für Streaming/SD-Writes (in Bytes).
   * Wert wird gerundet auf einen sinnvollen Block (z. B. 64/128/256/512).
   * @param bytes gewünschte Chunk-Größe.
   */
  void setChunkSize(size_t bytes);

  /**
   * Setzt Timeout (ms) für Capture-Operationen.
   * @param ms Timeout in Millisekunden.
   */
  void setTimeout(uint32_t ms);

  /**
   * Liefert das aktuell konfigurierte Chunk-Size (Bytes).
   * @return chunk size in Bytes.
   */
  size_t getChunkSize() const;

  /**
   * Liefert das zugrundeliegende ArduCAM-Objekt zur direkten Nutzung (falls nötig).
   * Achtung: direkte Nutzung kann die Wrapper-Invarianten verletzen.
   * @return Pointer auf internes ArduCAM-Objekt.
   */
  ArduCAM* camera();

private:
  uint8_t _csPin;
  uint8_t _i2cSda;
  uint8_t _i2cScl;
  uint32_t _spiClockHz;
  size_t _chunkSize;
  uint32_t _timeoutMs;

  // ArduCAM-Instanz (OV2640)
  ArduCAM _camera;

  // temporäre FIFO-Lese-Buffer (heap-allocated bei Bedarf)
  uint8_t* _readBuf;

  // interne Hilfsfunktionen
  bool waitForCaptureDone(uint32_t timeoutMs);
  bool readFIFOToSd(MicroSDCard &sd, uint32_t timeoutMs);
  bool readFIFOToCallback(bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user, uint32_t timeoutMs);
};

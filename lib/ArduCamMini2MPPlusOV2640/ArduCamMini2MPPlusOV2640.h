/**
 * @file ArduCamMini2MPPlusOV2640.h
 * @brief Wrapper für ArduCAM Mini 2MP Plus (OV2640)
 *
 * Dieser Header definiert die Klasse ArduCamMini2MPPlusOV2640, einen
 * hardware-nahen Wrapper um die ArduCAM-API (ArduCAM.h). Ziel ist, Capture,
 * FIFO-Burst-Reads, Streaming und SD-Persistenz in einer einfachen Schnittstelle
 * zu kapseln, kompatibel zur ArduCAM.h-Implementierung in deinem Projekt.
 *
 * Verwendung (Kurzform):
 *   ArduCamMini2MPPlusOV2640 cam(csPin, sda, scl);
 *   cam.begin();
 *   cam.captureToSD(sd, "/img.jpg");
 *
 * Alle public-Funktionen sind dokumentiert; private Helfer sind nur kurz kommentiert.
 */

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ArduCAM.h>
#include <memorysaver.h>

#include "MicroSDCard.h"

/**
 * @class ArduCamMini2MPPlusOV2640
 * @brief Speicher- und Capture-Wrapper für ArduCAM Mini 2MP Plus (OV2640)
 *
 * Die Klasse kapselt:
 *  - Initialisierung (I2C, SPI, Kamera-Init)
 *  - Capture-Vorgänge (Buffer, SD, Stream-Callback)
 *  - Robustere FIFO-Burst-Lese-Logik (chunked, timeoutfähig)
 *
 * Hinweis: Diese Wrapper erwartet die ArduCAM-Primitives wie in ArduCAM.h:
 * CS_LOW/CS_HIGH, set_fifo_burst(), transferBytes(), rdSensorReg8_8(reg,&out), wrSensorReg8_8(...)
 */
class ArduCamMini2MPPlusOV2640 {
public:
  /**
   * @brief Konstruktor
   * @param csPin SPI Chip Select Pin für die ArduCAM-Hardware
   * @param i2cSda I2C SDA Pin (Wire.begin), Standard 21
   * @param i2cScl I2C SCL Pin (Wire.begin), Standard 22
   * @param spiClockHz Wunsch-SPI-Takt für die Kamera (nur Hinweis; SPI.begin() wird aufgerufen)
   * @param chunkSize Lesepuffer-Größe in Bytes für chunked FIFO-Reads (Default 512)
   * @param timeoutMs Default Timeout (ms) für Capture/Read-Operationen (Default 5000)
   *
   * Konstruktor reserviert keine Hardware-Ressourcen; begin() muss aufgerufen werden.
   */
  explicit ArduCamMini2MPPlusOV2640(uint8_t csPin = 5,
                                    uint8_t i2cSda = 21,
                                    uint8_t i2cScl = 22,
                                    uint32_t spiClockHz = 8000000UL,
                                    size_t chunkSize = 512,
                                    uint32_t timeoutMs = 5000);

  /**
   * @brief Initialisiert I2C, SPI und führt eine minimale Kameraprüfung durch.
   * @return true bei erfolgreicher Initialisierung und wenn Kamera ID gelesen werden konnte,
   *         false wenn Kamera nicht ansprechbar ist oder SPI/Sensor-Init fehlgeschlagen ist.
   *
   * Ablauf:
   *  - Wire.begin(...) und SPI.begin()
   *  - ggf. Reset/Timing-Register setzen
   *  - versucht VID/PID via rdSensorReg8_8 zu lesen; bei 0x00/0x00 wird InitCAM() versucht und erneut geprüft
   *  - setzt Default-Format (JPEG) und säubert FIFO
   *
   * Fehler: Gibt false zurück wenn kein Sensor-Response nach Retry.
   */
  bool begin();

  /**
   * @brief Kurzer Präsenztest der Kamera.
   * @return true wenn ein Zeichen der Sensor-ID (VID oder PID) ungleich 0 gelesen wurde.
   *
   * Implementation: ruft rdSensorReg8_8 für VID/PID auf (richtige Pointer-Signatur wird verwendet).
   * Hinweis: Dies ist ein leichter Gesundheitstest; ein vollständiges Begin() macht mehr.
   */
  bool isCameraPresent();

  /**
   * @brief Setzt das Kameraformat auf JPEG und initialisiert die Kamera-Controller-Settings.
   * @return true bei erfolgreichem Aufruf (stateless, ruft ArduCAM set_format und InitCAM).
   */
  bool setFormatJPEG();

  /**
   * @brief Startet einen Capture und liest das komplette Bild in einen Heap-Puffer.
   * @param outBuf Zeiger auf Pufferzeiger, der nach erfolgreichem Aufruf auf ein malloc'd Buffer zeigt.
   *               Der Aufrufer ist verantwortlich für free(*outBuf).
   * @param outLen Referenz, die mit der Länge des Buffers (Bytes) gefüllt wird.
   * @param timeoutMs optionales Timeout in ms; 0 -> Default der Instanz (setTimeout).
   * @return true bei Erfolg, false bei Timeout, Allocation-Fehler oder Read-Fehler.
   *
   * Ablauf:
   *  - Start capture (flush/clear/start_capture)
   *  - Warte auf CAP_DONE_MASK via waitForCaptureDone
   *  - Lese read_fifo_length und allokiere Buffer
   *  - burstReadBytesArduCAM liest FIFO chunked in den Buffer
   */
  bool captureToBuffer(uint8_t** outBuf, size_t &outLen, uint32_t timeoutMs = 0);

  /**
   * @brief Nimmt ein Capture vor und speichert das Bild atomar auf SD (tmp -> rename).
   * @param sd Referenz auf MicroSDCard-Wrapper (bereits initialisiert)
   * @param path Zielpfad (z. B. "/capture.jpg")
   * @param timeoutMs optionales Timeout in ms; 0 -> Default der Instanz
   * @return true bei Erfolg, false bei Fehler (Capture, SD-Write, Rename)
   *
   * Ablauf:
   *  - Start capture und Warte auf Fertig
   *  - sd.streamWriteStart(path.tmp)
   *  - Lese FIFO in Chunks mittels readFIFOToSd und schreibe jeweils per streamWriteChunk
   *  - sd.streamWriteEnd() -> atomisches Umbenennen
   */
  bool captureToSD(MicroSDCard &sd, const char* path, uint32_t timeoutMs = 0);

  /**
   * @brief Nimmt ein Capture vor und liefert Chunked-Data an einen Callback.
   * @param cb Callback-Funktion: bool cb(const uint8_t* buf, size_t len, void* user)
   *           Rückgabe false bedeutet: Abbruch (z. B. Frame-Ende).
   * @param user optionaler user pointer, wird an cb weitergegeben
   * @param timeoutMs Timeout in ms; 0 -> Default der Instanz
   * @return true bei erfolgreichem Streaming, false bei Fehler/Timeout
   *
   * Ablauf:
   *  - Capture starten, warten, dann readFIFOToCallback für chunked reads.
   */
  bool captureStreaming(bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user = nullptr, uint32_t timeoutMs = 0);

  /**
   * @brief Sensor-Softreset (OV2640-kompatibel) implementiert via Sensor-Register.
   *
   * Implementation:
   *  - wrSensorReg8_8(0xFF, 0x01) (Bank select, falls erforderlich)
   *  - wrSensorReg8_8(0x12, 0x80) (COM7 reset)
   *  - kleine Delays zur Stabilisierung und FIFO cleanup
   */
  void softReset();

  /**
   * @brief Setzt die Chunk-Größe (für FIFO-Reads).
   * @param bytes gewünschte Chunk-Größe; wird gecappt und gerundet (min 64, max 4096, 64er-Block).
   *
   * Diese Chunk-Größe ist der Puffer, mit dem burstReadBytesArduCAM in Schleifen liest.
   * Kleinere Werte verringern Heap-Usage; größere Werte erhöhen Durchsatz bis zu RAM-Limits.
   */
  void setChunkSize(size_t bytes);

  /**
   * @brief Setzt das Default-Timeout für capture/read Operationen (ms).
   * @param ms Timeout Millisekunden
   */
  void setTimeout(uint32_t ms);

  /**
   * @brief Liefert die aktuell konfigurierte Chunk-Größe zurück.
   * @return Chunk-Größe in Bytes.
   */
  size_t getChunkSize() const;

  /**
   * @brief Liefert Pointer auf das interne ArduCAM-Objekt (z. B. für Spezialoperationen).
   * @return Zeiger auf ArduCAM
   *
   * Vorsicht: Direktaufrufe auf _camera sollten die Invarianten der Wrapper-Klasse bewahren.
   */
  ArduCAM* camera();

private:
  // Konfigurationsfelder
  uint8_t _csPin;
  uint8_t _i2cSda;
  uint8_t _i2cScl;
  uint32_t _spiClockHz;
  size_t _chunkSize;
  uint32_t _timeoutMs;

  // Internes ArduCAM-Objekt (vom Upstream-Header)
  ArduCAM _camera;

  // Wiederverwendeter read buffer für chunked reads (malloc/free im Hot-Path kontrolliert)
  uint8_t* _readBuf;

  /**
   * @brief Wartet bis Capture-Done-Flag gesetzt oder Timeout überschritten.
   * @param timeoutMs Timeout in ms
   * @return true wenn CAP_DONE_MASK gesetzt wurde, false bei Timeout
   */
  bool waitForCaptureDone(uint32_t timeoutMs);

  /**
   * @brief Liest das aktuelle FIFO und schreibt es chunked in die MicroSDCard (streamWriteChunk).
   * @param sd Referenz auf MicroSDCard
   * @param timeoutMs Timeout in ms (0 -> Default)
   * @return true bei Erfolg, false bei Fehler/Timeout
   *
   * Diese Funktion übernimmt die chunked-allokation/release von _readBuf und die
   * Aufrufe an burstReadBytesArduCAM.
   */
  bool readFIFOToSd(MicroSDCard &sd, uint32_t timeoutMs);

  /**
   * @brief Liest FIFO chunked und ruft für jeden Chunk cb(buf,len,user) auf.
   * @param cb Callback-Funktion
   * @param user User-Pointer
   * @param timeoutMs Timeout in ms
   * @return true bei Erfolg, false bei Fehler/Timeout
   */
  bool readFIFOToCallback(bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user, uint32_t timeoutMs);
};

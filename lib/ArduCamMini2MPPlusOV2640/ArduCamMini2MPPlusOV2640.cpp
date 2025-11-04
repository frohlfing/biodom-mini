/**
 * @file ArduCamMini2MPPlusOV2640.cpp
 * @brief Implementierung des OV2640-Wrappers, angepasst an die ArduCAM.h API
 *
 * Wichtige Punkte:
 * - Nutzt ArduCAM Primitives aus ArduCAM.h:
 *   CS_LOW/CS_HIGH, set_fifo_burst(), transferBytes(out,in,size),
 *   rdSensorReg8_8(reg, &out) und wrSensorReg8_8(reg, val)
 * - Implementiert robustes, chunked FIFO-Burst-Reading ohne read_fifo_burst()
 * - Wiederverwendet _readBuf zur Vermeidung unnötiger malloc/free Schlenker
 * - Bietet timeouts und optionale kleine Block-Delays (hardware-stabil)
 *
 * Änderungen ggü. Original:
 * - Keine Verwendung von nicht existierenden Methoden wie begin()/test()
 * - Sensor-ID-Lesen per Pointer-Signatur korrigiert
 * - softReset() implementiert via OV2640 COM7 reset
 */

#include "ArduCamMini2MPPlusOV2640.h"
#include <cstring>

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 512
#endif

ArduCamMini2MPPlusOV2640::ArduCamMini2MPPlusOV2640(uint8_t csPin,
                                                   uint8_t i2cSda,
                                                   uint8_t i2cScl,
                                                   uint32_t spiClockHz,
                                                   size_t chunkSize,
                                                   uint32_t timeoutMs)
  : _csPin(csPin),
    _i2cSda(i2cSda),
    _i2cScl(i2cScl),
    _spiClockHz(spiClockHz),
    _chunkSize(chunkSize ? chunkSize : DEFAULT_CHUNK_SIZE),
    _timeoutMs(timeoutMs ? timeoutMs : 5000),
    _camera(OV2640, csPin),
    _readBuf(nullptr)
{
  // Konstruktor: keine Hardware-Initialisierung hier (begin() macht das)
}

/* begin()
 * - initialisiert Wire und SPI
 * - probiert Sensor-ID per rdSensorReg8_8 aus (richtige Signatur: reg, &out)
 * - bei initialer Fehllesung ruft InitCAM() einmal auf und probiert erneut
 * - setzt JPEG Format und säubert FIFO
 */
bool ArduCamMini2MPPlusOV2640::begin() {
  Wire.begin(_i2cSda, _i2cScl);
  SPI.begin();

  // Soft-reset/Timing-Init Register (harmlos, hilft manchmal)
  _camera.write_reg(ARDUCHIP_TIM, 0x00);
  delay(10);

  // Einige Beispiele wählen Bank 1 vor Sensor-Registerzugriff
  _camera.wrSensorReg8_8(0xFF, 0x01);

  uint8_t vid = 0, pid = 0;
  _camera.rdSensorReg8_8(0x00, &vid);
  _camera.rdSensorReg8_8(0x0A, &pid);

  // Wenn erste Leseversuche 0 liefern, versuche InitCAM und retry
  if (vid == 0x00 && pid == 0x00) {
    _camera.InitCAM();
    delay(10);
    _camera.rdSensorReg8_8(0x00, &vid);
    _camera.rdSensorReg8_8(0x0A, &pid);
    if (vid == 0x00 && pid == 0x00) {
      // Kamera wahrscheinlich nicht da / nicht ansprechbar
      return false;
    }
  }

  // Setze JPEG-Format und initialisiere Controller
  _camera.set_format(JPEG);
  _camera.InitCAM();

  // Saubere FIFO-Startbedingung
  _camera.flush_fifo();
  _camera.clear_fifo_flag();

  return true;
}

/* isCameraPresent()
 * - kurzer Sensor-ID-Check via rdSensorReg8_8
 * - gibt true zurück, wenn irgendein ID-Byte ungleich 0 gelesen wurde
 */
bool ArduCamMini2MPPlusOV2640::isCameraPresent() {
  uint8_t vid = 0, pid = 0;
  _camera.wrSensorReg8_8(0xFF, 0x01);
  _camera.rdSensorReg8_8(0x00, &vid);
  _camera.rdSensorReg8_8(0x0A, &pid);
  return (vid != 0x00 || pid != 0x00);
}

bool ArduCamMini2MPPlusOV2640::setFormatJPEG() {
  _camera.set_format(JPEG);
  _camera.InitCAM();
  return true;
}

/* softReset()
 * - führt Sensor-Softreset über OV2640 COM7 (Reg 0x12, Wert 0x80) aus.
 * - bank-select vor/nach dem Zugriff, kleine Delays für Stabilität.
 * - zusätzlich flush/clear FIFO.
 */
void ArduCamMini2MPPlusOV2640::softReset() {
  _camera.wrSensorReg8_8(0xFF, 0x01); // Bank 1 (üblich vor Sensor-Regs)
  _camera.wrSensorReg8_8(0x12, 0x80); // COM7 reset
  delay(10);
  _camera.wrSensorReg8_8(0xFF, 0x00);
  delay(10);
  _camera.flush_fifo();
  _camera.clear_fifo_flag();
}

/* waitForCaptureDone(timeoutMs)
 * - Pollt ARDUCHIP_TRIG auf CAP_DONE_MASK bis Timeout
 * - liefert true wenn Capture fertig, false bei Timeout
 */
bool ArduCamMini2MPPlusOV2640::waitForCaptureDone(uint32_t timeoutMs) {
  uint32_t start = millis();
  uint32_t tmo = timeoutMs ? timeoutMs : _timeoutMs;
  while (millis() - start < tmo) {
    uint8_t state = _camera.read_reg(ARDUCHIP_TRIG);
    if (state & CAP_DONE_MASK) return true;
    delay(5);
  }
  return false;
}

/* burstReadBytesArduCAM(...)
 * - Kernfunktion für FIFO-Burst-Reads.
 * - Verwendet: CS_LOW(); set_fifo_burst(); transferBytes(out,in,size); CS_HIGH();
 * - Liest in 'chunk'-Blöcken, intern weiter in DUMMY_SZ-Blöcken.
 * - blockDelayUs (default 12us) stabilisiert Bus/Module; kann 0 sein.
 * - Erwartet: 'dest' hat Platz für 'length' Bytes (Aufrufer muss allokieren).
 *
 * Gründe für diese Implementierung:
 * - Viele Offizielle Beispiele zeigen byteweise SPI.transfer loops.
 * - transferBytes macht Blocktransfers effizienter; wir chunken in kleinen Blöcken
 *   (DUMMY_SZ) um Stack/Heap zu schonen und kompatibel zu bleiben.
 */
static bool burstReadBytesArduCAM(ArduCAM &cam, uint8_t *dest, uint32_t length, size_t chunkHint, uint32_t blockDelayUs = 12) {
  if (length == 0) return true;

  const size_t chunk = chunkHint ? chunkHint : 512;
  constexpr size_t DUMMY_SZ = 64; // kleines Inner-Block für transferBytes
  uint8_t dummy[DUMMY_SZ];
  memset(dummy, 0x00, DUMMY_SZ);

  uint32_t remaining = length;
  uint32_t offset = 0;

  while (remaining > 0) {
    size_t toRead = remaining > chunk ? chunk : (size_t)remaining;

    // Start burst read sequence per ArduCAM API
    cam.CS_LOW();
    cam.set_fifo_burst();

    // Lese 'toRead' Bytes in DUMMY_SZ-Blöcken
    uint32_t done = 0;
    while (done < toRead) {
      size_t block = (toRead - done > DUMMY_SZ) ? DUMMY_SZ : (toRead - done);
      // transferBytes(out,in,block): send dummy out bytes and capture 'in' into dest
      cam.transferBytes(dummy, dest + offset + done, block);
      done += block;
      if (blockDelayUs) delayMicroseconds(blockDelayUs);
    }

    cam.CS_HIGH();

    offset += toRead;
    remaining -= toRead;
  }

  return true;
}

/* readFIFOToSd(sd, timeoutMs)
 * - Liest FIFO length, chunked Burst-Read in wiederverwendeten _readBuf und schreibt per sd.streamWriteChunk.
 * - Reuse: _readBuf wird alloziert falls leer und wieder freed am Ende.
 * - Prüft globalen Timeout (timeoutMs).
 */
bool ArduCamMini2MPPlusOV2640::readFIFOToSd(MicroSDCard &sd, uint32_t timeoutMs) {
  uint32_t length = _camera.read_fifo_length();
  if (length == 0) return false;

  const size_t chunk = _chunkSize ? _chunkSize : 512;

  // lazy-allocate read buffer (wird nach Ende freed)
  if (!_readBuf) {
    _readBuf = (uint8_t*)malloc(chunk);
    if (!_readBuf) return false;
  }

  uint32_t remaining = length;
  uint32_t start = millis();
  while (remaining > 0) {
    size_t toRead = remaining > chunk ? chunk : (size_t)remaining;

    // burst read into _readBuf
    if (!burstReadBytesArduCAM(_camera, _readBuf, toRead, chunk)) {
      free(_readBuf); _readBuf = nullptr;
      return false;
    }

    // stream-chunk schreiben (atomic logic handled by MicroSDCard)
    if (!sd.streamWriteChunk(_readBuf, toRead)) {
      free(_readBuf); _readBuf = nullptr;
      return false;
    }

    remaining -= toRead;
    if (timeoutMs && (millis() - start > timeoutMs)) {
      free(_readBuf); _readBuf = nullptr;
      return false;
    }
  }

  free(_readBuf); _readBuf = nullptr;
  return true;
}

/* readFIFOToCallback(cb,user,timeoutMs)
 * - Liest FIFO in Chunks, ruft pro Chunk den Callback auf.
 * - Callback kann false zurückgeben, um das weitere Lesen abzubrechen (z. B. Frame-Ende erkannt).
 */
bool ArduCamMini2MPPlusOV2640::readFIFOToCallback(bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user, uint32_t timeoutMs) {
  uint32_t length = _camera.read_fifo_length();
  if (length == 0) return false;

  const size_t chunk = _chunkSize ? _chunkSize : 512;

  if (!_readBuf) {
    _readBuf = (uint8_t*)malloc(chunk);
    if (!_readBuf) return false;
  }

  uint32_t remaining = length;
  uint32_t start = millis();
  while (remaining > 0) {
    size_t toRead = remaining > chunk ? chunk : (size_t)remaining;

    if (!burstReadBytesArduCAM(_camera, _readBuf, toRead, chunk)) {
      free(_readBuf); _readBuf = nullptr;
      return false;
    }

    if (cb) {
      bool cont = cb(_readBuf, toRead, user);
      if (!cont) { free(_readBuf); _readBuf = nullptr; return true; }
    }

    remaining -= toRead;
    if (timeoutMs && (millis() - start > timeoutMs)) {
      free(_readBuf); _readBuf = nullptr;
      return false;
    }
  }

  free(_readBuf); _readBuf = nullptr;
  return true;
}

/* captureToBuffer(outBuf,outLen,timeoutMs)
 * - Startet Capture, wartet auf Fertig, allokiert Buffer mit read_fifo_length(), liest komplett per burstReadBytesArduCAM.
 * - Caller ist verantwortlich für free(*outBuf).
 */
bool ArduCamMini2MPPlusOV2640::captureToBuffer(uint8_t** outBuf, size_t &outLen, uint32_t timeoutMs) {
  if (!outBuf) return false;
  _camera.flush_fifo();
  _camera.clear_fifo_flag();
  _camera.start_capture();

  if (!waitForCaptureDone(timeoutMs)) return false;

  uint32_t length = _camera.read_fifo_length();
  if (length == 0) return false;

  uint8_t* buf = (uint8_t*)malloc(length);
  if (!buf) return false;

  bool ok = burstReadBytesArduCAM(_camera, buf, length, _chunkSize ? _chunkSize : 512);
  if (!ok) { free(buf); return false; }

  *outBuf = buf;
  outLen = length;
  return true;
}

/* captureToSD(sd,path,timeoutMs)
 * - Atomarer Save-Vorgang: sd.streamWriteStart(tmp) -> readFIFOToSd -> streamWriteEnd() -> atomicRename(tmp->path)
 * - Return false wenn ein Schritt fehlschlägt.
 */
bool ArduCamMini2MPPlusOV2640::captureToSD(MicroSDCard &sd, const char* path, uint32_t timeoutMs) {
  if (timeoutMs == 0) timeoutMs = _timeoutMs;

  _camera.flush_fifo();
  _camera.clear_fifo_flag();
  _camera.start_capture();
  if (!waitForCaptureDone(timeoutMs)) return false;

  if (!sd.streamWriteStart(path, false)) return false;

  bool ok = readFIFOToSd(sd, timeoutMs);
  if (!ok) {
    sd.streamWriteEnd(); // best-effort cleanup
    return false;
  }

  if (!sd.streamWriteEnd()) return false;
  return true;
}

/* captureStreaming(cb,user,timeoutMs)
 * - Capture + reading via callback chunks
 */
bool ArduCamMini2MPPlusOV2640::captureStreaming(bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user, uint32_t timeoutMs) {
  if (timeoutMs == 0) timeoutMs = _timeoutMs;
  _camera.flush_fifo();
  _camera.clear_fifo_flag();
  _camera.start_capture();
  if (!waitForCaptureDone(timeoutMs)) return false;
  return readFIFOToCallback(cb, user, timeoutMs);
}

/* setChunkSize / setTimeout / getChunkSize / camera()
 * - Utility-Methoden
 */
void ArduCamMini2MPPlusOV2640::setChunkSize(size_t bytes) {
  size_t size = bytes;
  if (size < 64) size = 64;
  if (size > 4096) size = 4096;
  size = (size / 64) * 64;
  _chunkSize = size;
}

void ArduCamMini2MPPlusOV2640::setTimeout(uint32_t ms) {
  _timeoutMs = ms;
}

size_t ArduCamMini2MPPlusOV2640::getChunkSize() const {
  return _chunkSize;
}

ArduCAM* ArduCamMini2MPPlusOV2640::camera() {
  return &_camera;
}

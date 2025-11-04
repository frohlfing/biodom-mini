/**
 * @file MicroSDCard.cpp
 * Implementierung der MicroSDCard-Klasse (SdFat-only).
 *
 * Double-buffered streamWrite: Puffergröße konfigurierbar in 512B-Sektoren.
 */

#include "MicroSDCard.h"

static constexpr size_t SECTOR_SIZE = 512;

MicroSDCard::MicroSDCard(uint8_t csPin, size_t bufferSectors)
  : _sd(), _streamFile(), _cs(csPin), _spiClockHz(4000000UL),
    _bufferSectors(bufferSectors), _bufA(nullptr), _bufB(nullptr),
    _fillBuf(nullptr), _writeBuf(nullptr), _fillPos(0), _streamActive(false) {}

bool MicroSDCard::begin(uint32_t spiClockHz) {
  _spiClockHz = spiClockHz;
  SPI.begin();
  // SdFat: initialisieren mit CS und SCK-MHz. SD_SCK_MHZ erwartet MHz-Wert.
  if (!_sd.begin(_cs, SD_SCK_MHZ(_spiClockHz / 1000000U))) {
    return false;
  }
  if (!allocateBuffers()) return false;
  return true;
}

void MicroSDCard::setSpiClock(uint32_t hz) {
  _spiClockHz = hz;
}

bool MicroSDCard::allocateBuffers() {
  freeBuffers();
  size_t bytes = _bufferSectors * SECTOR_SIZE;
  if (bytes == 0) return false;
  _bufA = static_cast<uint8_t*>(malloc(bytes));
  _bufB = static_cast<uint8_t*>(malloc(bytes));
  if (!_bufA || !_bufB) { freeBuffers(); return false; }
  _fillBuf = _bufA;
  _writeBuf = _bufB;
  _fillPos = 0;
  return true;
}

void MicroSDCard::freeBuffers() {
  if (_bufA) { free(_bufA); _bufA = nullptr; }
  if (_bufB) { free(_bufB); _bufB = nullptr; }
  _fillBuf = _writeBuf = nullptr;
  _fillPos = 0;
}

bool MicroSDCard::exists(const char* path) {
  return _sd.exists(path);
}

bool MicroSDCard::remove(const char* path) {
  if (!_sd.exists(path)) return false;
  return _sd.remove(path);
}

String MicroSDCard::tmpPath(const char* path) const {
  String p(path);
  return p + ".tmp";
}

bool MicroSDCard::atomicRename(const char* tmpPath, const char* finalPath) {
  if (_sd.exists(finalPath)) {
    if (!_sd.remove(finalPath)) {
      _sd.remove(tmpPath);
      return false;
    }
  }
  if (_sd.rename(tmpPath, finalPath)) return true;

  // Fallback: manuelles Kopieren
  File32 t = _sd.open(tmpPath, O_READ);
  if (!t) return false;
  File32 f = _sd.open(finalPath, O_WRITE | O_TRUNC | O_CREAT);
  if (!f) { t.close(); return false; }
  const size_t bufSize = 512;
  uint8_t buf[bufSize];
  while (t.available()) {
    size_t r = t.read(buf, bufSize);
    f.write(buf, r);
  }
  t.close();
  f.close();
  _sd.remove(tmpPath);
  return true;
}

bool MicroSDCard::readText(const char* path, String &outText) {
  outText = String();
  if (!_sd.exists(path)) return false;
  File32 f = _sd.open(path, O_READ);
  if (!f) return false;
  const size_t bufSize = 256;
  char buf[bufSize];
  while (f.available()) {
    size_t r = f.read(buf, bufSize);
    outText += String(buf).substring(0, r);
  }
  f.close();
  return true;
}

bool MicroSDCard::saveText(const char* path, const char* text, bool append) {
  String tmp = tmpPath(path);
  if (append && _sd.exists(path)) {
    File32 orig = _sd.open(path, O_READ);
    if (!orig) return false;
    File32 t = _sd.open(tmp.c_str(), O_WRITE | O_TRUNC | O_CREAT);
    if (!t) { orig.close(); return false; }
    const size_t bufSize = 512;
    uint8_t buf[bufSize];
    while (orig.available()) {
      size_t r = orig.read(buf, bufSize);
      t.write(buf, r);
    }
    orig.close();
    t.close();
  }
  File32 f = _sd.open(tmp.c_str(), O_WRITE | O_APPEND | O_CREAT);
  if (!f) return false;
  size_t written = f.write((const uint8_t*)text, strlen(text));
  f.close();
  if (written != strlen(text)) { _sd.remove(tmp.c_str()); return false; }
  return atomicRename(tmp.c_str(), path);
}

bool MicroSDCard::saveBinary(const char* path, const uint8_t* data, size_t len, bool /*append*/) {
  if (!data || len == 0) return false;
  String tmp = tmpPath(path);
  File32 f = _sd.open(tmp.c_str(), O_WRITE | O_TRUNC | O_CREAT);
  if (!f) return false;
  const size_t chunk = 512;
  size_t written = 0;
  while (written < len) {
    size_t w = (len - written) > chunk ? chunk : (len - written);
    size_t w2 = f.write(data + written, w);
    if (w2 != w) { f.close(); _sd.remove(tmp.c_str()); return false; }
    written += w2;
  }
  f.close();
  return atomicRename(tmp.c_str(), path);
}

bool MicroSDCard::streamWriteStart(const char* path, bool /*append*/) {
  if (_streamActive) streamWriteEnd();
  _tmpPathForWrite = tmpPath(path);
  _streamFile = _sd.open(_tmpPathForWrite.c_str(), O_WRITE | O_TRUNC | O_CREAT);
  if (!_streamFile) return false;
  _fillPos = 0;
  _streamActive = true;
  return true;
}

bool MicroSDCard::streamWriteChunk(const uint8_t* data, size_t len) {
  if (!_streamActive) return false;
  size_t bytesLeft = len;
  size_t offset = 0;
  size_t bufBytes = _bufferSectors * SECTOR_SIZE;
  while (bytesLeft > 0) {
    size_t space = bufBytes - _fillPos;
    size_t toCopy = bytesLeft < space ? bytesLeft : space;
    memcpy(_fillBuf + _fillPos, data + offset, toCopy);
    _fillPos += toCopy;
    offset += toCopy;
    bytesLeft -= toCopy;

    if (_fillPos == bufBytes) {
      // Swap buffers
      uint8_t* toWrite = _fillBuf;
      _fillBuf = (_fillBuf == _bufA) ? _bufB : _bufA;
      _writeBuf = toWrite;
      _fillPos = 0;
      size_t wrote = _streamFile.write(_writeBuf, bufBytes);
      if (wrote != bufBytes) {
        _streamFile.close();
        _sd.remove(_tmpPathForWrite.c_str());
        _streamActive = false;
        return false;
      }
    }
  }
  return true;
}

bool MicroSDCard::streamWriteEnd() {
  if (!_streamActive) return false;
  // Restdaten schreiben
  if (_fillPos > 0) {
    size_t wrote = _streamFile.write(_fillBuf, _fillPos);
    if (wrote != _fillPos) {
      _streamFile.close();
      _sd.remove(_tmpPathForWrite.c_str());
      _streamActive = false;
      return false;
    }
  }
  _streamFile.close();
  _streamActive = false;
  String finalPath = _tmpPathForWrite;
  if (finalPath.endsWith(".tmp")) {
    String orig = finalPath.substring(0, finalPath.length() - 4);
    return atomicRename(finalPath.c_str(), orig.c_str());
  }
  return false;
}

bool MicroSDCard::readBinary(const char* path, bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user) {
  if (!_sd.exists(path)) return false;
  File32 f = _sd.open(path, O_READ);
  if (!f) return false;
  const size_t bufSize = 512;
  static uint8_t buf[bufSize];
  while (f.available()) {
    size_t r = f.read(buf, bufSize);
    if (cb) {
      bool cont = cb(buf, r, user);
      if (!cont) { f.close(); return true; }
    }
  }
  f.close();
  return true;
}

void MicroSDCard::listDir(const char* path, void(*cb)(const char* name, bool isDir)) {
  File32 dir = _sd.open(path, O_READ);
  if (!dir) return;
  File32 entry;
  while ((entry = dir.openNextFile())) {
    if (cb) cb(entry.name(), entry.isDirectory());
    entry.close();
  }
  dir.close();
}

bool MicroSDCard::isCardPresent() {
  // Leichtgewichtiger Präsenzcheck: versuche Root zu öffnen
  return _sd.begin(_cs);
}

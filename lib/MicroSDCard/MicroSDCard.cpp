/**
 * @file MicroSDCard.cpp
 * @brief Implementierung der MicroSDCard-Klasse (SdFat-only)
 *
 * Hinweise:
 * - Diese Implementierung verwendet SdFat (SdFs auf ESP32) und erwartet, dass
 *   die Bibliothek korrekt im Projekt verfügbar ist (greiman/SdFat).
 * - FsFile wird als Dateityp genutzt (SdFs auf ESP32 liefert FsFile).
 * - streamWriteStart/Chunk/End implementieren ein double-buffered Schreiben
 *   mit Puffergröße = bufferSectors * 512 bytes.
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
  // SdFat expects SCK in MHz macro
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

/* atomicRename(tmp, final)
 * - Versucht zuerst _sd.rename(tmp, final) (atomar)
 * - Falls nicht möglich: Fallback-Kopie via FsFile (open/read/write)
 * - Entfernt tmp nach erfolgreichem Kopieren
 * - Entfernt tmp (und ggf. partial final) bei Fehler
 */
bool MicroSDCard::atomicRename(const char* tmpPath, const char* finalPath) {
  if (_sd.exists(finalPath)) {
    if (!_sd.remove(finalPath)) {
      _sd.remove(tmpPath);
      return false;
    }
  }
  if (_sd.rename(tmpPath, finalPath)) return true;

  // Fallback: manuelles Kopieren mit FsFile
  FsFile t = _sd.open(tmpPath, O_READ);
  if (!t) return false;
  FsFile f = _sd.open(finalPath, O_WRITE | O_TRUNC | O_CREAT);
  if (!f) { t.close(); return false; }

  const size_t bufSize = 512;
  uint8_t buf[bufSize];
  while (t.available()) {
    int r = t.read(buf, bufSize);
    if (r <= 0) break;
    int w = f.write(buf, r);
    if (w != r) {
      t.close(); f.close();
      _sd.remove(tmpPath);
      _sd.remove(finalPath);
      return false;
    }
  }
  t.close();
  f.close();
  _sd.remove(tmpPath);
  return true;
}

/* readText(path, outText)
 * - Liest eine Textdatei in Blöcken und hängt sie an outText an.
 * - Benutzt FsFile (SdFs).

 * Achtung: String kann Heap-Fragmentierung verursachen bei großen Dateien.
 */
bool MicroSDCard::readText(const char* path, String &outText) {
  outText = String();
  if (!_sd.exists(path)) return false;
  FsFile f = _sd.open(path, O_READ);
  if (!f) return false;
  const size_t bufSize = 256;
  char buf[bufSize];
  while (f.available()) {
    int r = f.read(buf, bufSize);
    if (r <= 0) break;
    outText += String(buf).substring(0, r);
  }
  f.close();
  return true;
}

/* saveText(path,text,append)
 * - Atomare Speicherung: schreibt in tmp, benennt tmp->final um
 * - Wenn append: kopiere zuerst existierende Datei in tmp, dann füge text an
 */
bool MicroSDCard::saveText(const char* path, const char* text, bool append) {
  String tmp = tmpPath(path);
  if (append && _sd.exists(path)) {
    FsFile orig = _sd.open(path, O_READ);
    if (!orig) return false;
    FsFile t = _sd.open(tmp.c_str(), O_WRITE | O_TRUNC | O_CREAT);
    if (!t) { orig.close(); return false; }
    const size_t bufSize = 512;
    uint8_t buf[bufSize];
    while (orig.available()) {
      int r = orig.read(buf, bufSize);
      if (r <= 0) break;
      int w = t.write(buf, r);
      if (w != r) { orig.close(); t.close(); _sd.remove(tmp.c_str()); return false; }
    }
    orig.close();
    t.close();
  }
  FsFile f = _sd.open(tmp.c_str(), O_WRITE | O_APPEND | O_CREAT);
  if (!f) return false;
  size_t textLen = strlen(text);
  size_t written = f.write((const uint8_t*)text, textLen);
  f.close();
  if (written != textLen) { _sd.remove(tmp.c_str()); return false; }
  return atomicRename(tmp.c_str(), path);
}

/* saveBinary(path,data,len)
 * - Atomare Speicherung wie saveText
 */
bool MicroSDCard::saveBinary(const char* path, const uint8_t* data, size_t len, bool /*append*/) {
  if (!data || len == 0) return false;
  String tmp = tmpPath(path);
  FsFile f = _sd.open(tmp.c_str(), O_WRITE | O_TRUNC | O_CREAT);
  if (!f) return false;
  const size_t chunk = 512;
  size_t written = 0;
  while (written < len) {
    size_t w = (len - written) > chunk ? chunk : (len - written);
    int w2 = f.write(data + written, w);
    if (w2 != (int)w) { f.close(); _sd.remove(tmp.c_str()); return false; }
    written += w2;
  }
  f.close();
  return atomicRename(tmp.c_str(), path);
}

/* streamWriteStart/Chunk/End
 * - Double-buffered streaming: füllt _fillBuf; wenn voll, tauscht und schreibt _writeBuf
 * - Puffergröße = _bufferSectors * 512
 * - Bei Fehler: entfernt tmp
 */
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
      // buffer voll -> swap und schreiben
      uint8_t* toWrite = _fillBuf;
      _fillBuf = (_fillBuf == _bufA) ? _bufB : _bufA;
      _writeBuf = toWrite;
      _fillPos = 0;
      int wrote = _streamFile.write(_writeBuf, bufBytes);
      if (wrote != (int)bufBytes) {
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
    int wrote = _streamFile.write(_fillBuf, _fillPos);
    if (wrote != (int)_fillPos) {
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

/* readBinary(path,cb)
 * - Liest eine Datei in 512-Bytes Blöcken und ruft Callback auf.
 */
bool MicroSDCard::readBinary(const char* path, bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user) {
  if (!_sd.exists(path)) return false;
  FsFile f = _sd.open(path, O_READ);
  if (!f) return false;
  const size_t bufSize = 512;
  static uint8_t buf[bufSize];
  while (f.available()) {
    int r = f.read(buf, bufSize);
    if (r <= 0) break;
    if (cb) {
      bool cont = cb(buf, r, user);
      if (!cont) { f.close(); return true; }
    }
  }
  f.close();
  return true;
}

/* listDir(path,cb)
 * - iteriert Verzeichnis-Einträge und ruft cb auf
 */
void MicroSDCard::listDir(const char* path, void(*cb)(const char* name, bool isDir)) {
  FsFile dir = _sd.open(path, O_READ);
  if (!dir) return;
  FsFile entry;
  while ((entry = dir.openNextFile())) {
    if (cb) cb(entry.name(), entry.isDirectory());
    entry.close();
  }
  dir.close();
}

/* isCardPresent()
 * - Leichter Präsenztest: versuche sd.begin mit CS
 * - Beachte: das kann nebenbei initialisieren; call begin() bevorzugt
 */
bool MicroSDCard::isCardPresent() {
  return _sd.begin(_cs);
}

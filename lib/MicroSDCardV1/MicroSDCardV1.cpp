#include "MicroSDCardV1.h"

MicroSDCardV1::MicroSDCardV1(uint8_t csPin)
  : _cs(csPin), _streamFile(), _tmpPathForWrite(), _spiClockHz(4000000UL) {}

bool MicroSDCardV1::begin(uint32_t spiClockHz) {
  _spiClockHz = spiClockHz;
  SPI.begin();
  // SD.begin nimmt CS-Pin und optional SPI-Objekt
  if (!SD.begin(_cs, SPI, _spiClockHz)) {
    return false;
  }
  return true;
}

bool MicroSDCardV1::exists(const char* path) {
  return SD.exists(path);
}

bool MicroSDCardV1::remove(const char* path) {
  if (!SD.exists(path)) return false;
  return SD.remove(path);
}

bool MicroSDCardV1::readText(const char* path, String &outText) {
  outText = String();
  if (!SD.exists(path)) return false;
  File f = SD.open(path, FILE_READ);
  if (!f) return false;
  // Leesen in Chunks, um RAM zu schonen
  const size_t bufSize = 256;
  char buf[bufSize];
  while (f.available()) {
    size_t r = f.readBytes(buf, bufSize);
    outText += String(buf).substring(0, r);
  }
  f.close();
  return true;
}

bool MicroSDCardV1::saveText(const char* path, const char* text, bool append) {
  // Atomic write: schreibe tmp und rename
  String tmp = tmpPath(path);
  File f;
  if (append && SD.exists(path)) {
    // offene Anhängung: direkt anhängen ist ok, aber wir fallbacks auf tmp+cat um atomar zu bleiben
    f = SD.open(tmp.c_str(), FILE_WRITE);
    if (!f) {
      // erstelle tmp und kopiere vorhandene Datei davor
      File orig = SD.open(path, FILE_READ);
      if (orig) {
        File t = SD.open(tmp.c_str(), FILE_WRITE);
        if (!t) { orig.close(); return false; }
        const size_t bufSize = 256;
        uint8_t buf[bufSize];
        while (orig.available()) {
          size_t r = orig.read(buf, bufSize);
          t.write(buf, r);
        }
        orig.close();
        t.close();
        f = SD.open(tmp.c_str(), FILE_WRITE);
        if (!f) return false;
      } else {
        // kein Original, lege tmp neu an
        f = SD.open(tmp.c_str(), FILE_WRITE);
        if (!f) return false;
      }
    }
  } else {
    // neu schreiben
    f = SD.open(tmp.c_str(), FILE_WRITE);
    if (!f) return false;
  }

  f.write((const uint8_t*)text, strlen(text));
  f.close();

  // Atomarer Ersetzungsmechanismus
  return atomicRename(tmp.c_str(), path);
}

bool MicroSDCardV1::saveBinary(const char* path, const uint8_t* data, size_t len, bool append) {
  if (!data || len == 0) return false;
  String tmp = tmpPath(path);
  File f = SD.open(tmp.c_str(), FILE_WRITE);
  if (!f) return false;
  // Schreibe in Chunks (falls len sehr groß)
  const size_t chunk = 512;
  size_t written = 0;
  while (written < len) {
    size_t w = (len - written) > chunk ? chunk : (len - written);
    f.write(data + written, w);
    written += w;
  }
  f.close();
  return atomicRename(tmp.c_str(), path);
}

bool MicroSDCardV1::streamWriteStart(const char* path, bool append) {
  if (_streamFile) {
    _streamFile.close();
  }
  _tmpPathForWrite = tmpPath(path);
  // Offen im WRITE Mode (neu)
  _streamFile = SD.open(_tmpPathForWrite.c_str(), FILE_WRITE);
  return bool(_streamFile);
}

bool MicroSDCardV1::streamWriteChunk(const uint8_t* data, size_t len) {
  if (!_streamFile) return false;
  size_t w = _streamFile.write(data, len);
  return (w == len);
}

bool MicroSDCardV1::streamWriteEnd() {
  if (!_streamFile) return false;
  _streamFile.close();
  String finalPath = _tmpPathForWrite;
  // finalPath war tmp, rekonstruiere echten Pfad: tmpPath() hängt ".tmp" an
  if (finalPath.endsWith(".tmp")) {
    String orig = finalPath.substring(0, finalPath.length() - 4);
    return atomicRename(finalPath.c_str(), orig.c_str());
  }
  return false;
}

bool MicroSDCardV1::readBinary(const char* path, bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user) {
  if (!SD.exists(path)) return false;
  File f = SD.open(path, FILE_READ);
  if (!f) return false;
  const size_t bufSize = 512;
  static uint8_t buf[bufSize];
  while (f.available()) {
    size_t r = f.read(buf, bufSize);
    if (cb) {
      bool cont = cb(buf, r, user);
      if (!cont) {
        f.close();
        return true;
      }
    }
  }
  f.close();
  return true;
}

void MicroSDCardV1::listDir(const char* path, void(*cb)(const char* name, bool isDir)) {
  File dir = SD.open(path);
  if (!dir) return;
  File entry;
  while ((entry = dir.openNextFile())) {
    if (cb) cb(entry.name(), entry.isDirectory());
    entry.close();
  }
  dir.close();
}

bool MicroSDCardV1::isCardPresent() {
  // Einfacher Test: versuche Root zu öffnen.
  return SD.begin();
}

/** Hilfsfunktionen */

String MicroSDCardV1::tmpPath(const char* path) const {
  String p(path);
  return p + ".tmp";
}

bool MicroSDCardV1::atomicRename(const char* tmpPath, const char* finalPath) {
  // Entferne bestehende Ziel-Datei, falls vorhanden
  if (SD.exists(finalPath)) {
    if (!SD.remove(finalPath)) {
      // Falls Löschen fehlschlägt, entferne tmp und return false
      SD.remove(tmpPath);
      return false;
    }
  }
  // Umbenennen (move)
  bool ok = SD.rename(tmpPath, finalPath);
  if (!ok) {
    // Fallback: versuche Kopieren
    File t = SD.open(tmpPath, FILE_READ);
    if (!t) return false;
    File f = SD.open(finalPath, FILE_WRITE);
    if (!f) { t.close(); return false; }
    const size_t bufSize = 512;
    uint8_t buf[bufSize];
    while (t.available()) {
      size_t r = t.read(buf, bufSize);
      f.write(buf, r);
    }
    t.close();
    f.close();
    SD.remove(tmpPath);
    ok = true;
  }
  return ok;
}

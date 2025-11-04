/**
 * @file MicroSDCard.h
 * @brief SdFat-basierter MicroSD-Wrapper (kompatibel mit SdFs auf ESP32)
 *
 * Diese Klasse kapselt:
 *  - atomare Dateioperationen (tmp -> rename mit Fallback-Kopie)
 *  - double-buffered streaming writer (konfigurierbar in 512B-Sektoren)
 *  - simple utilities (exists, remove, listDir, readBinary)
 *
 * Hinweis: Implementierung verwendet SdFat (greiman/SdFat). Auf ESP32 repräsentiert
 *       SdFs-Dateioperationen den Typ FsFile.
 */

#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include <FS.h>    // FsFile für SdFs auf ESP32

/**
 * @class MicroSDCard
 * @brief Lokale Kapselung für den MicroSD-Kartenleser mit SdFat
 *
 * Designentscheidungen:
 *  - atomare Speicherung über tmp-Datei + rename; falls rename nicht verfügbar: Fallback
 *    auf manuelle Kopie (öffnen/lesen/schreiben).
 *  - streamWriteStart/Chunk/End für große Datenströme (Double-Buffer).
 */
class MicroSDCard {
public:
  /**
   * @brief Konstruktor
   * @param csPin SPI-CS Pin für das SD-Modul
   * @param bufferSectors Anzahl 512-Byte Sektoren pro internem Puffer (Default 1 => 512 B)
   *
   * Konstruktor initialisiert interne Zeiger, allokiert aber keine Buffers.
   * Ruf begin() auf, um SPI/SdFat zu initialisieren und Buffers zu erstellen.
   */
  explicit MicroSDCard(uint8_t csPin, size_t bufferSectors = 1);

  /**
   * @brief Initialisiert SPI und SdFat.
   * @param spiClockHz SPI-Takt in Hz (z.B. 4000000UL). SdFat erwartet SCK in MHz via SD_SCK_MHZ.
   * @return true bei Erfolg, false bei Initialisierungsfehler (z. B. keine Karte)
   *
   * On success this function also allocates the internal double buffers via allocateBuffers().
   */
  bool begin(uint32_t spiClockHz = 4000000UL);

  /**
   * @brief Setzt SPI-Takt (nur Konfigurationswert; begin muss erneut aufgerufen werden, damit SdFat ihn nutzt).
   * @param hz SCLK in Hz
   */
  void setSpiClock(uint32_t hz);

  /**
   * @brief Prüft, ob eine Datei oder ein Pfad auf der Karte existiert.
   * @param path Pfad zur Datei
   * @return true wenn vorhanden, false sonst
   */
  bool exists(const char* path);

  /**
   * @brief Löscht eine Datei, falls vorhanden.
   * @param path Pfad zur Datei
   * @return true bei Erfolg, false bei Fehler oder wenn Datei nicht existiert
   */
  bool remove(const char* path);

  /**
   * @brief Liest eine Textdatei vollständig in einen Arduino String.
   * @param path Pfad zur Datei
   * @param outText Referenz auf String, der mit Inhalt gefüllt wird
   * @return true bei Erfolg, false bei Fehler (z. B. Datei nicht vorhanden)
   *
   * Achtung: große Dateien können Heap-fragmentierung verursachen; für große Binaries
   *         besser readBinary + Callback verwenden.
   */
  bool readText(const char* path, String &outText);

  /**
   * @brief Speichert Text atomar auf der Karte (tmp -> rename).
   * @param path Zielpfad
   * @param text Null-terminierter C-String mit Inhalt
   * @param append wenn true, wird existierende Datei zuerst in tmp kopiert und text angehängt
   * @return true bei Erfolg, false bei Fehlern (I/O, Memory)
   */
  bool saveText(const char* path, const char* text, bool append = true);

  /**
   * @brief Speichert ein Binary-Blob atomar (tmp -> rename).
   * @param path Zielpfad
   * @param data Pointer auf Daten
   * @param len Länge in Bytes
   * @param append reserviert (derzeit ignoriert)
   * @return true bei Erfolg, false bei Fehlern
   */
  bool saveBinary(const char* path, const uint8_t* data, size_t len, bool append = false);

  /**
   * @brief Startet einen stream-basierten Schreibvorgang in eine temporäre Datei.
   * @param path Zielpfad (intern path + ".tmp")
   * @param append reserviert (derzeit nicht unterstützt)
   * @return true wenn tmp-Datei geöffnet wurde, false sonst
   *
   * Ablauf: streamWriteStart -> streamWriteChunk* -> streamWriteEnd
   */
  bool streamWriteStart(const char* path, bool append = false);

  /**
   * @brief Schreibt ein Chunk in den laufenden Stream. Puffer werden intern accumuliert.
   * @param data Buffer mit Daten
   * @param len Länge in Bytes
   * @return true bei Erfolg, false falls Stream nicht aktiv oder Schreibfehler
   *
   * Intern wird ein fill-buffer gefüllt; wenn dieser voll ist, wird er asynchron ersetzt
   * und das volle Buffer synchron auf Kartendatei geschrieben.
   */
  bool streamWriteChunk(const uint8_t* data, size_t len);

  /**
   * @brief Schließt den Stream, schreibt Restdaten und benennt tmp -> final (atomar).
   * @return true bei Erfolg, false bei Fehler (z. B. Schreibfehler oder Rename-Fehler)
   */
  bool streamWriteEnd();

  /**
   * @brief Liest eine Binärdatei chunked und ruft cb(buf,len,user) auf.
   * @param path Pfad zur Datei
   * @param cb Callback, das den Buffer verarbeitet; Rückgabe false bricht das Lesen ab.
   * @param user Optionaler user pointer, wird an cb weitergereicht
   * @return true bei Erfolg (oder bei vorzeitigem Abbruch durch cb), false bei Fehler
   */
  bool readBinary(const char* path, bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user = nullptr);

  /**
   * @brief Listet Einträge eines Verzeichnisses und ruft cb(name,isDir) für jeden Eintrag auf.
   * @param path Verzeichnis (z. B. "/")
   * @param cb Callback pro Eintrag (Name, isDirectory)
   */
  void listDir(const char* path, void(*cb)(const char* name, bool isDir));

  /**
   * @brief Leichter Präsenztest der Karte (ruft intern _sd.begin mit CS).
   * @return true wenn Karte erreichbar scheint, false sonst
   *
   * Hinweis: begin() ist vollständiger und allokiert Buffers; isCardPresent ist ein
   * schneller health-check.
   */
  bool isCardPresent();

private:
  SdFat _sd;           ///< SdFat-Instanz
  FsFile _streamFile;  ///< offenes Stream-File (SdFs -> FsFile auf ESP32)
  uint8_t _cs;         ///< CS-Pin
  uint32_t _spiClockHz;

  // Double-buffer intern
  size_t _bufferSectors;
  uint8_t* _bufA;
  uint8_t* _bufB;
  uint8_t* _fillBuf;
  uint8_t* _writeBuf;
  size_t _fillPos;
  bool _streamActive;
  String _tmpPathForWrite;

  /**
   * @brief Erzeugt temporären Pfad: original + ".tmp"
   * @param path Originalpfad
   * @return String mit tmp-Pfad
   */
  String tmpPath(const char* path) const;

  /**
   * @brief Atomisches Umbenennen tmp -> final, mit Fallback auf manuelles Kopieren.
   * @param tmpPath Pfad der temporären Datei
   * @param finalPath gewünschter Zielpfad
   * @return true bei Erfolg, false bei Fehlern
   *
   * Ablauf:
   *  - wenn final existiert: entferne final
   *  - versuche _sd.rename(tmp, final)
   *  - falls rename fehlschlägt: Öffne tmp/final und kopiere Blockweise
   *  - entferne tmp nach Erfolg; bei Fehlern versuche aufräumen
   */
  bool atomicRename(const char* tmpPath, const char* finalPath);

  /**
   * @brief Allokiert die beiden internen Buffers (_bufA/_bufB) in Größe bufferSectors * 512.
   * @return true bei Erfolg, false bei OOM oder bufferSectors==0
   */
  bool allocateBuffers();

  /**
   * @brief Gibt alle internen Buffers frei und setzt State zurück.
   */
  void freeBuffers();
};

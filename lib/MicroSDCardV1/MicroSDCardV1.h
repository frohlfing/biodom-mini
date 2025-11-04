/**
 * @file MicroSDCardV1.h
 * @brief Lokale Kapselung für den MicroSD-Kartenleser.
 */

#pragma once
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

/**
 * @brief Klasse für den MicroSD-Kartenleser.
 */
class MicroSDCardV1 {
public:
  /**
   * Konstruktor
   * @param csPin SPI Chip Select Pin für das SD-Modul
   */
  explicit MicroSDCardV1(uint8_t csPin);

  /**
   * Initialisiert SPI und SdFat.
   * @param spiClockHz SPI SCLK in Hz (z. B. 4000000UL). Bei langen Kabeln langsamer wählen.
   * @return true bei erfolgreicher Initialisierung, false ansonsten.
   */
  bool begin(uint32_t spiClockHz = 4000000UL);

  /**
   * Prüft, ob eine Datei existiert.
   * @param path Pfad, z. B. "/log.txt"
   * @return true wenn vorhanden, false sonst.
   */
  bool exists(const char* path);

  /**
   * Löscht eine Datei.
   * @param path Pfad der zu löschenden Datei.
   * @return true bei Erfolg, false bei Fehler oder nicht vorhanden.
   */
  bool remove(const char* path);

  /**
   * Liest eine Textdatei vollständig und schreibt den Inhalt in outText.
   * @param path Pfad der Datei.
   * @param outText Referenz, in die der Text geschrieben wird.
   * @return true bei Erfolg, false bei Fehler oder nicht vorhanden.
   */
  bool readText(const char* path, String &outText);

  /**
   * Speichert Text atomar (tmp -> rename). Bei append==true wird der Text angehängt.
   * @param path Zielpfad.
   * @param text Null-terminierter C-String mit Inhalt.
   * @param append true: anhängen, false: überschreiben.
   * @return true bei Erfolg, false bei Fehler.
   */
  bool saveText(const char* path, const char* text, bool append = true);

  /**
   * Schreibt ein Binary-Blob atomar (tmp -> rename).
   * @param path Zielpfad.
   * @param data Pointer auf die Daten.
   * @param len Länge in Bytes.
   * @param append Nicht verwendet in dieser Implementierung (für zukünftige Erweiterung reserviert).
   * @return true bei Erfolg, false bei Fehler.
   */
  bool saveBinary(const char* path, const uint8_t* data, size_t len, bool append = false);

  /**
   * Startet einen Stream-Write in eine temporäre Datei. Reihenfolge:
   * streamWriteStart(path) -> streamWriteChunk(...) -> streamWriteEnd()
   * Intern: double-buffered auf Sektor-Ebene.
   * @param path Zielpfad (wird intern als path.tmp angelegt).
   * @param append reserved (derzeit nicht unterstützt).
   * @return true bei Erfolg (Stream geöffnet), false sonst.
   */
  bool streamWriteStart(const char* path, bool append = false);

  /**
   * Schreibt ein Chunk in den Stream. Kann mehrfach aufgerufen werden.
   * @param data Puffer mit Daten.
   * @param len Länge der Daten in Bytes.
   * @return true bei Erfolg, false bei Schreibfehler oder wenn Stream nicht aktiv ist.
   */
  bool streamWriteChunk(const uint8_t* data, size_t len);

  /**
   * Schließt den Stream, schreibt Reste und benennt tmp -> final (atomar).
   * @return true bei Erfolg, false bei Fehler.
   */
  bool streamWriteEnd();

  /**
   * Liest eine Binärdatei in Chunks und ruft cb(buf,len,user) auf.
   * Wenn cb false zurückgibt, wird das Lesen vorzeitig beendet.
   * @param path Pfad der Datei.
   * @param cb Callback, der Daten verarbeitet.
   * @param user Optionaler User-Pointer, wird an cb übergeben.
   * @return true bei Erfolg, false bei Fehler.
   */
  bool readBinary(const char* path, bool (*cb)(const uint8_t* buf, size_t len, void* user), void* user = nullptr);

  /**
   * Listet ein Verzeichnis und ruft cb(name,isDir) pro Eintrag auf.
   * @param path Verzeichnis-Pfad, z. B. "/".
   * @param cb Callback pro Eintrag.
   */
  void listDir(const char* path, void(*cb)(const char* name, bool isDir));

  /**
   * Führt einen einfachen Präsenztest für die Karte aus.
   * @return true wenn Karte erreichbar scheint, false sonst.
   */
  bool isCardPresent();

private:
  uint8_t _cs;
  File _streamFile;
  String _tmpPathForWrite;
  uint32_t _spiClockHz;

  /** Helfer: erstellt tmp-Pfad (append ".tmp"). */
  String tmpPath(const char* path) const;

  /** Helfer: atomische Umbenennung tmp -> final (mit Fallback auf Kopie). */
  bool atomicRename(const char* tmpPath, const char* finalPath);
};

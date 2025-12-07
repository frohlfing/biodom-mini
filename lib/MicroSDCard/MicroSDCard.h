#pragma once

#include <Arduino.h>
#include <SD.h> // Die spezifische Implementierung für SD-Karten.

/**
 * Klasse für den MicroSD SPI Kartenleser.
 * 
 * Kapselt die Initialisierung und die grundlegenden Datei- und Verzeichnisoperationen
 * für ein SPI-basiertes MicroSD-Kartenmodul.
 */
class MicroSDCard
{
public:
    /**
     * @brief Konstruktor der MicroSDCard-Klasse.
     * @param csPin Der GPIO-Pin, der als Chip Select für das SD-Modul verwendet wird.
     */
    explicit MicroSDCard(uint8_t csPin);

    /**
     * @brief Initialisiert das SD-Kartenmodul.
     * Muss im setup() des Hauptprogramms aufgerufen werden.
     * @return true bei erfolgreicher Initialisierung, andernfalls false.
     */
    bool begin();

    /**
     * @return true, wenn die SD-Karte bereit ist
     */
    bool isReady() const;

    // --- Verzeichnisoperationen ---

     /**
     * @brief Listet den Inhalt eines Verzeichnisses (Dateien und Unterverzeichnisse) auf.
     * @param dirname Der Pfad des Verzeichnisses, das aufgelistet werden soll (z.B. "/data").
     * @param output Ein Stream-Objekt (z.B. Serial), in das die Auflistung geschrieben wird.
     * @return true, wenn das Verzeichnis erfolgreich geöffnet wurde, andernfalls false.
     */
    static bool listDir(const char* dirname, Stream &output);

    /**
     * Version von listDir mit Callback
     * @param dirname Der Pfad des Verzeichnisses, das aufgelistet werden soll (z.B. "/data").
     * @param callback
     * @return true, wenn das Verzeichnis erfolgreich geöffnet wurde, andernfalls false.
     */
    bool listDir(const char* dirname, const std::function<void(const String& filename, size_t size)> &callback) const;

    /**
     * @brief Erstellt ein neues Verzeichnis.
     * @param path Der vollständige Pfad des zu erstellenden Verzeichnisses (z.B. "/logs/2024").
     * @return true bei Erfolg, false bei einem Fehler (z.B. Pfad existiert bereits).
     */
    static bool createDir(const char* path);

    /**
     * @brief Entfernt ein leeres Verzeichnis.
     * @param path Der vollständige Pfad des zu löschenden Verzeichnisses.
     * @return true bei Erfolg, false bei einem Fehler (z.B. Verzeichnis nicht gefunden oder nicht leer).
     */
    static bool removeDir(const char* path);

    /**
     * @brief Löscht alle Dateien (keine Ordner) in einem gegebenen Verzeichnis.
     * @param dirname Der Pfad des Verzeichnisses, das geleert werden soll (z.B. "/").
     * @return true, wenn alle Löschvorgänge erfolgreich waren, sonst false.
     */
    bool deleteAllFilesInDir(const char* dirname) const;

    // --- Dateioperationen ---

    /**
     * @brief Öffnet eine Datei zum Lesen und gibt das File-Objekt zurück.
     * @param path Der Pfad zur Datei.
     * @return Ein File-Objekt. Wenn die Datei nicht geöffnet werden kann, ist das Objekt "false".
     */
    static File openFileForReading(const char* path);

    /**
     * @brief Liest den Inhalt einer Datei und leitet ihn in einen Stream um.
     * @param path Der Pfad zur Datei.
     * @param output Der Ziel-Stream (z.B. Serial).
     * @return true bei Erfolg, andernfalls false.
     */
    static bool readFile(const char* path, Stream &output);

    /**
     * @brief Liest den gesamten Inhalt einer Datei und gibt ihn als String zurück.
     * @param path Der Pfad zur Datei.
     * @return Ein String mit dem Dateiinhalt. Bei einem Fehler wird ein leerer String zurückgegeben.
     */
    static String readFile(const char* path);

    /**
     * @brief Öffnet eine Datei zum Schreiben und gibt das File-Objekt zurück.
     * @param path Der Pfad zur Datei.
     * @return Ein File-Objekt. Wenn die Datei nicht geöffnet werden kann, ist das Objekt "false".
     */
    static File openFileForWriting(const char* path);

    /**
     * @brief Schreibt einen Text in eine Datei.
     * Erstellt die Datei, falls sie nicht existiert. Überschreibt den Inhalt, falls sie existiert.
     * @param path Der Pfad zur Datei.
     * @param message Der Text, der in die Datei geschrieben werden soll.
     * @return true bei Erfolg, false, wenn die Datei nicht zum Schreiben geöffnet werden konnte.
     */
    static bool writeFile(const char* path, const char* message);

    /**
     * @brief Fügt einen Text am Ende einer bestehenden Datei an.
     * Erstellt die Datei, falls sie nicht existiert.
     * @param path Der Pfad zur Datei.
     * @param message Der Text, der angefügt werden soll.
     * @return true bei Erfolg, false, wenn die Datei nicht zum Anhängen geöffnet werden konnte.
     */
    static bool appendFile(const char* path, const char* message);

    /**
     * @brief Schreibt Daten aus einem Stream in eine Datei auf SD.
     *        Arbeitet nicht-blockierend mit Buffer und prüft optional auf ein Endezeichen.
     * @param path Pfad zur Zieldatei.
     * @param input Eingabe-Stream (z.B. Serial, WiFiClient).
     * @param bufferSize Größe des Schreibpuffers (Standard: 64 Bytes).
     * @param endMarker Optionales Endezeichen (z.B. "###END###"). Wenn nullptr, wird bis Stream-Ende geschrieben.
     * @return true, wenn Daten geschrieben wurden, false wenn nichts verfügbar.
     */
    static bool processStreamChunk(const char *path, Stream &input, size_t bufferSize, const char *endMarker);

    /**
     * @brief Benennt eine Datei um oder verschiebt sie.
     * @param path1 Der ursprüngliche Pfad der Datei.
     * @param path2 Der neue Pfad der Datei.
     * @return true bei Erfolg, false bei einem Fehler.
     */
    static bool renameFile(const char* path1, const char* path2);

    /**
     * @brief Löscht eine Datei.
     * @param path Der Pfad der zu löschenden Datei.
     * @return true bei Erfolg, false bei einem Fehler (z.B. Datei nicht gefunden).
     */
    static bool deleteFile(const char* path);

    // --- Informationen abfragen ---

    /**
     * @brief Gibt den Typ der SD-Karte zurück.
     * @return Ein String, der den Kartentyp beschreibt (z.B. "SDHC").
     */
    static String getCardType();
    
    /**
     * @brief Gibt die physische Gesamtgröße der SD-Karte in Megabyte zurück.
     * @return Die Größe der SD-Karte in Megabyte.
     */
    static uint64_t getCardSizeMB();
    
    /**
     * @brief Gibt den gesamten nutzbaren Speicherplatz des Dateisystems in Megabyte zurück.
     * @return Der gesamte nutzbare Speicherplatz in Megabyte.
     */
    static uint64_t getTotalSpaceMB();
    
    /**
     * @brief Gibt den belegten Speicherplatz des Dateisystems in Megabyte zurück.
     * @return Der belegte Speicherplatz in Megabyte.
     */
    static uint64_t getUsedSpaceMB();

private:
    uint8_t _csPin; // Speicher für den Chip Select Pin
    bool _isReady;
};
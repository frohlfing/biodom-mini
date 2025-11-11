/**
 * Funktionstest für den SD Kartenleser (MicroSD SPI Module)
 * 
 * Basierend auf dem offiziellen Beispiel der ESP32 Arduino Core SD-Bibliothek:
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD/examples/SD_Test
 *
 * Dieser Sketch demonstriert die grundlegende Nutzung des SD-Kartenlesers
 * über die Arduino SD-Bibliothek auf einem ESP32.
 *
 * Die meisten Hilfsfunktionen nehmen ein Argument vom Typ `fs::FS &fs`. 
 * Dies ist eine Referenz auf ein Dateisystem-Objekt (`FS`). 
 * `SD` ist eine Instanz davon. Dieser Ansatz macht die Funktionen universell, 
 * da man ihnen auch andere Dateisysteme wie `SPIFFS` oder `LittleFS` übergeben könnte.
 *
 * Weitere Details zur SD-Bibliothek auf einem ESP32 siehe:
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 */

// --- Benötigte Bibliotheken ---
#include <SD.h>   // Die spezifische Implementierung für SD-Karten.

// --- Optionale Neuzuweisung der SPI-Pins ---
// Wenn dieses Makro aktiviert ist, kann man alternative GPIOs für SPI festlegen.
// Im Standardfall wird dies nicht benötigt, da die Hardware-SPI-Pins des ESP32 verwendet werden.
#define REASSIGN_PINS
int sck = -1;   // Standard-SCK-Pin (GPIO 18)
int miso = -1;  // Standard-MISO-Pin (GPIO 19)
int mosi = -1;  // Standard-MOSI-Pin (GPIO 23)
int cs = 16;    // Chip Select für die SD-Karte (hier GPIO 16)

// --- Hilfsfunktionen für Dateioperationen ---
// Diese Funktionen kapseln die jeweilige Funktionalität und geben Statusmeldungen aus.
// Sie sind universell geschrieben und akzeptieren eine Referenz auf ein beliebiges Dateisystem (&fs).

// Listet den Inhalt eines Verzeichnisses auf.
void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.path(), levels - 1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

// Erstellt ein neues Verzeichnis.
void createDir(fs::FS &fs, const char *path) {
    Serial.printf("Creating Dir: %s\n", path);
    if (fs.mkdir(path)) {
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

// Entfernt ein Verzeichnis.
void removeDir(fs::FS &fs, const char *path) {
    Serial.printf("Removing Dir: %s\n", path);
    if (fs.rmdir(path)) {
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

// Liest den Inhalt einer Datei und gibt ihn seriell aus.
void readFile(fs::FS &fs, const char *path) {
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while (file.available()) {
        Serial.write(file.read());
    }
    file.close();
}

// Schreibt eine Nachricht in eine Datei (überschreibt existierenden Inhalt).
void writeFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

// Fügt eine Nachricht am Ende einer Datei an.
void appendFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    if (file.print(message)) {
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

// Benennt eine Datei um.
void renameFile(fs::FS &fs, const char *path1, const char *path2) {
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

// Löscht eine Datei.
void deleteFile(fs::FS &fs, const char *path) {
    Serial.printf("Deleting file: %s\n", path);
    if (fs.remove(path)) {
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

// Führt einen einfachen Lese-/Schreib-Benchmark durch.
void testFileIO(fs::FS &fs, const char *path) {
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if (file) {
        len = file.size();
        size_t flen = len;
        start = millis();
        while (len) {
            size_t toRead = len;
            if (toRead > 512) {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %lu ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }

    file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for (i = 0; i < 2048; i++) {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
    file.close();
}

#define LED 5  // GPIO5 für die Status-LED

// =======================================================================================
// SETUP-Funktion: Wird einmal beim Start ausgeführt
// =======================================================================================
void setup() {
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    Serial.begin(115200);

    // SD-Karte initialisieren
    if (!SD.begin(cs)) {
        Serial.println("Card Mount Failed");
        return; // Abbruch, wenn die Karte nicht initialisiert werden kann.
    }

    // Abbruch, wenn keine Karte erkannt wird
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    // Typ der Karte anzeigen
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    // Kartengröße anzeigen
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    // Dateioperationen testen
    Serial.println("Starting test sequence...");
    listDir(SD, "/", 0);
    createDir(SD, "/mydir");
    listDir(SD, "/", 0);
    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
    Serial.println("Test sequence finished.");

    // Speicherplatz anzeigen
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void loop() {
    // Nichts zu tun im Loop, alles passiert im Setup.
}

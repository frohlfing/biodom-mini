#include "MicroSDCard.h"

MicroSDCard::MicroSDCard(const uint8_t csPin) : _csPin(csPin) {}

bool MicroSDCard::begin() const {
    return SD.begin(_csPin);
}

bool MicroSDCard::listDir(const char* dirname, Stream &output) {
    File root = SD.open(dirname);
    if (!root || !root.isDirectory()) {
        return false;
    }
    
    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            output.print("  DIR : ");
            output.println(file.name());
        } else {
            output.print("  FILE: ");
            output.print(file.name());
            output.print("\tSIZE: ");
            output.println(file.size());
        }
        file = root.openNextFile();
    }
    return true;
}

bool MicroSDCard::createDir(const char* path) {
    return SD.mkdir(path);
}

bool MicroSDCard::removeDir(const char* path) {
    return SD.rmdir(path);
}

File MicroSDCard::openFileForReading(const char* path) {
    return SD.open(path, FILE_READ);
}

bool MicroSDCard::readFile(const char* path, Stream &output) {
    File file = SD.open(path, FILE_READ);
    if (!file) {
        return false;
    }
    constexpr size_t bufferSize = 256;
    uint8_t buffer[bufferSize];
    while (file.available()) {
        const size_t bytesRead = file.read(buffer, bufferSize);
        output.write(buffer, bytesRead);
    }
    file.close();
    return true;
}

String MicroSDCard::readFile(const char* path) {
    File file = SD.open(path, FILE_READ);
    if (!file) {
        return ""; // Leerer String bei Fehler
    }
    String content = file.readString();
    file.close();
    return content;
}

File MicroSDCard::openFileForWriting(const char* path) {
    return SD.open(path, FILE_WRITE);
}

bool MicroSDCard::writeFile(const char* path, const char* message) {
    // "w" für Write, true für Create if not exists
    File file = SD.open(path, FILE_WRITE, true);
    if (!file) {
        return false;
    }
    const bool success = file.print(message);
    file.close();
    return success;
}

bool MicroSDCard::appendFile(const char* path, const char* message) {
    File file = SD.open(path, FILE_APPEND, true);
    if (!file) {
        return false;
    }
    const bool success = file.print(message);
    file.close();
    return success;
}

bool MicroSDCard::processStreamChunk(const char* path, Stream &input, size_t bufferSize = 64, const char* endMarker = nullptr)
{
    if (!input.available()) {
        return false; // nichts zu tun
    }

    File file = SD.open(path, FILE_WRITE);
    if (!file) {
        return false;
    }

    // Buffer für effizientes Schreiben
    char buf[bufferSize];
    size_t count = 0;

    while (input.available() && count < bufferSize) {
        char c = input.read();
        buf[count++] = c;

        // Endezeichen prüfen (falls gesetzt)
        if (endMarker != nullptr) {
            static size_t matchPos = 0;
            if (c == endMarker[matchPos]) {
                matchPos++;
                if (endMarker[matchPos] == '\0') {
                    // Endmarker komplett erkannt → Datei schließen
                    file.write((uint8_t*)buf, count);
                    file.close();
                    return true;
                }
            } else {
                matchPos = 0; // Reset bei Fehlmatch
            }
        }
    }

    // Buffer schreiben
    if (count > 0) {
        file.write((uint8_t*)buf, count);
    }

    file.close();
    return true;
}

bool MicroSDCard::renameFile(const char* path1, const char* path2) {
    return SD.rename(path1, path2);
}

bool MicroSDCard::deleteFile(const char* path) {
    return SD.remove(path);
}

String MicroSDCard::getCardType() {
    const uint8_t type = SD.cardType();
    if (type == CARD_MMC) return "MMC";
    if (type == CARD_SD) return "SDSC";
    if (type == CARD_SDHC) return "SDHC";
    return "UNKNOWN";
}

uint64_t MicroSDCard::getCardSizeMB() {
    return SD.cardSize() / (1024 * 1024);
}

uint64_t MicroSDCard::getTotalSpaceMB() {
    return SD.totalBytes() / (1024 * 1024);
}

uint64_t MicroSDCard::getUsedSpaceMB() {
    return SD.usedBytes() / (1024 * 1024);
}
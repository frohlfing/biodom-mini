#include "MicroSDCard.h"

MicroSDCard::MicroSDCard(const uint8_t csPin) : _csPin(csPin) {}

bool MicroSDCard::begin() {
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

File MicroSDCard::openFileForWriting(const char* path) {
    return SD.open(path, FILE_WRITE);
}

bool MicroSDCard::readFile(const char* path, Stream &output) {
    File file = SD.open(path);
    if (!file) {
        return false;
    }
    
    const size_t bufferSize = 256;
    uint8_t buffer[bufferSize];
    size_t bytesRead;
    
    while (file.available()) {
        bytesRead = file.read(buffer, bufferSize);
        output.write(buffer, bytesRead);
    }
    file.close();
    return true;
}

String MicroSDCard::readFile(const char* path) {
    File file = SD.open(path, FILE_READ);
    if (!file) {
        return String(""); // Leerer String bei Fehler
    }
    String content = file.readString();
    file.close();
    return content;
}

bool MicroSDCard::writeFile(const char* path, const char* message) {
    // "w" für Write, true für Create if not exists
    File file = SD.open(path, "w", true); 
    if (!file) {
        return false;
    }
    bool success = file.print(message);
    file.close();
    return success;
}

bool MicroSDCard::appendFile(const char* path, const char* message) {
    File file = SD.open(path, "a", true);
    if (!file) {
        return false;
    }
    bool success = file.print(message);
    file.close();
    return success;
}

bool MicroSDCard::renameFile(const char* path1, const char* path2) {
    return SD.rename(path1, path2);
}

bool MicroSDCard::deleteFile(const char* path) {
    return SD.remove(path);
}

String MicroSDCard::getCardType() {
    uint8_t type = SD.cardType();
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
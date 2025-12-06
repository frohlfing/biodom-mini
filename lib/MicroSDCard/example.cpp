/**
 * Beispiel zur Nutzung der MicroSDCard-Bibliothek.
 * 
 * Das Beispiel initialisiert eine SD-Karte und führt dann eine Reihe von
 * Datei- und Verzeichnisoperationen aus, um die Funktionalität der
 * Bibliothek zu demonstrieren. Alle Aktionen finden einmalig im setup() statt.
 * Die Ergebnisse werden auf dem Seriellen Monitor ausgegeben.
 */

#include <Arduino.h>
#include "MicroSDCard.h"

// GPIO-Pin für den SPI Chip Select
constexpr uint8_t SD_CS_PIN = 16;

// Erstelle eine Instanz der Bibliotheksklasse
MicroSDCard sdCard(SD_CS_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("Starte MicroSDCard-Beispiel...");

    // CS-Pin auf HIGH setzen und SPI-Bus initialisieren
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    SPI.begin();

    // SD-Karte initialisieren
    if (!sdCard.begin()) {
        Serial.println("Initialisierung fehlgeschlagen! Programm wird angehalten.");
        while (true) { // Endlosschleife, um das Programm anzuhalten
            delay(100); // Prozessor entlasten
        }
    }
    Serial.println("SD-Karte erfolgreich initialisiert.");

    // --- Testsequenz ---

    Serial.println("\n--- Karteninformationen ---");
    Serial.printf("Typ: %s, Größe: %llu MB\n", MicroSDCard::getCardType().c_str(), MicroSDCard::getCardSizeMB());
    Serial.printf("Speicherplatz: %llu MB von %llu MB belegt\n", MicroSDCard::getUsedSpaceMB(), MicroSDCard::getTotalSpaceMB());

    Serial.println("\n--- Teste Verzeichnis- und Dateioperationen ---");
    
    Serial.println("\n1. Inhalt des Wurzelverzeichnisses auflisten:");
    MicroSDCard::listDir("/", Serial);

    Serial.println("\n2. Datei '/example.txt' schreiben...");
    if (MicroSDCard::writeFile("/example.txt", "Dies ist ein Test.")) {
        Serial.println("Schreiben erfolgreich.");
    } else {
        Serial.println("Schreiben fehlgeschlagen.");
    }

    Serial.println("\n3. Datei '/example.txt' lesen:");
    String content = MicroSDCard::readFile("/example.txt");
    if (content != "") {
        Serial.print("Inhalt: '");
        Serial.print(content);
        Serial.println("'");
    } else {
        Serial.println("Lesen fehlgeschlagen oder Datei leer.");
    }

    Serial.println("\n4. Text an '/example.txt' anhängen...");
    MicroSDCard::appendFile("/example.txt", "\nEine neue Zeile.");
    
    Serial.println("\n5. Datei erneut lesen, um Anhang zu prüfen:");
    content = MicroSDCard::readFile("/example.txt");
    Serial.print("Neuer Inhalt: '");
    Serial.print(content);
    Serial.println("'");

    Serial.println("\n6. Datei umbenennen zu '/renamed.txt'...");
    MicroSDCard::renameFile("/example.txt", "/renamed.txt");

    Serial.println("\n7. Inhalt des Wurzelverzeichnisses erneut auflisten:");
    MicroSDCard::listDir("/", Serial);

    Serial.println("\n8. Datei '/renamed.txt' löschen...");
    MicroSDCard::deleteFile("/renamed.txt");

    Serial.println("\n--- Testsequenz abgeschlossen ---");
}

void loop() {
    // Bleibt leer, alle Aktionen sind im setup()
}
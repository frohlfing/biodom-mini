/**
 * Capture-Test für die ArduCAM Mini 2MP Plus (mit OV2640-Chip)
 *
 * Basierend auf https://github.com/ArduCAM/Arducam_mini/tree/main/examples/mini/ArduCAM_Mini_2MP_Plus_Multi_Capture2SD, (C)2017 Lee
 *
 * Dieser Sketch demonstriert, wie man Bilder aufnimmt und auf einer SD-Karte speichert:
 * 
 * Er nimmt in einer Endlosschleife immer wieder ein Bild auf, misst die dafür benötigte Zeit, speichert das Bild
 * auf einer SD-Karte, misst auch diese Zeit und macht dann eine Pause von 5 Sekunden, bevor er von vorne beginnt.
 */

#include <Wire.h>     // Für die I2C-Kommunikation. Die Kamera wird über I2C konfiguriert (Auflösung, Effekte etc.).
#include <SPI.h>      // Für SPI-Kommunikation. Die Bilddaten werden schnell über SPI aus dem FIFO-Puffer gelesen.
#include <FS.h>       // Filesystem-Abstraktion, wird von der SD-Bibliothek auf dem ESP32 benötigt
#include <SD.h>       // Für die Kommunikation mit der SD-Karte

// Im Original-Sketch sollte man die ArduCAM-Bibliothek für die Hardware anpassen, indem man in memorysaver.h das Kameramodell einkommentiert. 
// Das ist uncool, ich definiere hier direkt die Hardware und lasse die Hersteller-Bibliothek unangetastet.
//#include "memorysaver.h" // nicht aufrufen, hat ja nicht die richtige Kamera definiert
#define _MEMORYSAVER_ // Verhindert, dass memorysaver.h die Kamera-Konfiguration ändert.
#define OV2640_MINI_2MP_PLUS // Definiert das Kameramodell als OV2640 Mini 2MP Plus
#include <ArduCAM.h>  // Die Hauptbibliothek für die Steuerung der ArduCAM-Hardware.

// Definiert die Anzahl der Frames für den Burst-Modus (hier nicht aktiv genutzt).
#define FRAMES_NUM 0x06

// --- Pin-Definitionen ---
const int CS = 17;      // Chip Select für die ArduCAM
#define SD_CS 16        // Chip Select für den SD-Kartenleser

// --- Globale Variablen ---
bool is_header = false; // Hilfsvariable zum Finden des JPEG-Starts
int total_time = 0;   // Variable für die Zeitmessung

// ArduCAM-Objekt instanziieren
ArduCAM myCAM( OV2640, CS );

// Funktions-Prototyp
uint8_t read_fifo_burst(ArduCAM myCAM);

// =======================================================================================
// SETUP-Funktion
// =======================================================================================
void setup() {
	uint8_t vid, pid; // Variablen für die Chip-ID der Kamera (Hersteller- & Produkt-ID)
	uint8_t temp; 	  // Temporäre Variable für Lese-Operationen

 	// I2C-Bus für die Kamerakonfiguration initialisieren.
	// Bei einigen Boards wie dem SAMD21/SAM3X muss man Wire1 statt Wire verwenden.
#if defined(__SAM3X8E__)
  Wire1.begin();
#else
  Wire.begin();
#endif

	// Serielle Schnittstelle für die Kommunikation mit dem PC starten.
  Serial.begin(115200);
  Serial.println(F("ArduCAM Start!"));

  // SPI-Bus initialisieren
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();

 	// --- Initialisierung und Überprüfung der ArduCAM-Hardware ---

  // 1. CPLD (Hilfs-Chip auf der ArduCAM) zurücksetzen.
  myCAM.write_reg(0x07, 0x80);
  delay(100);
  myCAM.write_reg(0x07, 0x00);
  delay(100);

  // 2. SPI-Verbindung zur ArduCAM testen
  while (1) {
		// Schreibe einen bekannten Wert (0x55) in ein Testregister und lese ihn sofort wieder aus.
    myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
    temp = myCAM.read_reg(ARDUCHIP_TEST1);
    if (temp != 0x55) {
			// Der gelesene Wert stimmt nicht überein, die SPI-Kommunikation ist fehlerhaft!
      Serial.println(F("SPI interface Error!"));
      delay(1000);
      continue; // Wiederhole die Schleife nach einer Pause.
    } else {
      Serial.println(F("SPI interface OK."));
      break;
    }
  }

  // 3. Kamerasensor-Modell überprüfen
  while (1) {    
    // Lese die Hersteller-ID (vid) und Produkt-ID (pid) des Kamerasensors aus.
    myCAM.wrSensorReg8_8(0xff, 0x01);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
    myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
    // Vergleiche die IDs mit den erwarteten Werten für einen OV2640.
    if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))) {
      Serial.println(F("ACK CMD Can't find OV2640 module!"));
      delay(1000);
      continue;
    }
    else {
      Serial.println(F("ACK CMD OV2640 detected."));
      break;
    }
  }

  // --- SD-Karten-Initialisierung ---
  Serial.println(F("Initializing SD card..."));
  while (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card Error! Retrying...")); 
    delay(1000);
  }
  Serial.println(F("SD Card detected."));

  // Typ der SD-Karte ausgeben
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
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

  // Speichergröße der SD-Karte in MB ausgeben
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // --- Kamera-Konfiguration ---
 	// Setze das Ausgabeformat auf JPEG. Alternativen wären z.B. BMP oder RAW.
  myCAM.set_format(JPEG);
	// Führt die grundlegende Initialisierungssequenz für die Kamera durch.
  myCAM.InitCAM();
	// Setze die Standard-Auflösung für JPEG-Bilder beim Start.
  myCAM.clear_fifo_flag();
  // Setze das "Aufnahme fertig"-Flag zurück, um die Kamera in einen sauberen, definierten Zustand zu versetzen. 
  myCAM.write_reg(ARDUCHIP_FRAMES, FRAMES_NUM);
}

// =======================================================================================
// LOOP-Funktion
// =======================================================================================
void loop() {
  // 1. Kamera für eine neue Aufnahme vorbereiten
  myCAM.flush_fifo();
  myCAM.clear_fifo_flag();

  // 2. Auflösung fest einstellen
  //myCAM.OV2640_set_JPEG_size(OV2640_160x120); // minimale Auflösung
  myCAM.OV2640_set_JPEG_size(OV2640_1600x1200); // maximale Auflösung
  delay(100); // Kurze Pause nach dem Ändern der Auflösung

  // 3. Aufnahme starten und Zeit messen
  myCAM.start_capture();
  Serial.println(F("Start capture."));
  total_time = millis();
  while ( !myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK));  // Warten bis Bild im FIFO ist
  total_time = millis() - total_time;
  Serial.print(F("Capture time used (in ms):"));
  Serial.println(total_time, DEC);

  // 4. Bild aus dem FIFO lesen, auf SD-Karte speichern und Zeit messen
  Serial.println(F("Start saving file to SD card..."));
  total_time = millis();
  read_fifo_burst(myCAM);
  total_time = millis() - total_time;
  Serial.print(F("Save time used (in ms):"));
  Serial.println(total_time, DEC);
  
  // 5. Status zurücksetzen und warten
  myCAM.clear_fifo_flag();
  Serial.println(F("------------------------------------"));
  delay(5000); // 5 Sekunden Pause bis zum nächsten Bild
}

// =======================================================================================
// Liest FIFO-Daten in einen Puffer und schreibt diesen auf die SD-Karte
//
// Arbeitsweise: 
// Die Funktion liest Bytes aus dem Kamera-FIFO in einen kleinen, temporären Puffer im RAM. 
// Sobald dieser Puffer voll ist (oder das Bildende erreicht wird), wird der gesamte Puffer
// auf einmal in eine Datei auf der SD-Karte geschrieben. Dieser Vorgang wird wiederholt, 
// bis das gesamte Bild übertragen ist.
// =======================================================================================
uint8_t read_fifo_burst(ArduCAM myCAM) {
  uint8_t temp = 0, temp_last = 0;
  uint32_t length = 0;

  // `static` Variablen behalten ihren Wert zwischen Funktionsaufrufen.
    static int i = 0;  // `i` ist der Index für den Puffer.
  static int k = 0;  // `k` zählt die Bilder hoch, um eindeutige Dateinamen zu erzeugen.
  char str[16];
  File outFile;

  // Ein Puffer von 256 Bytes im RAM, um Daten zwischenzuspeichern.
  byte buf[256];

  length = myCAM.read_fifo_length();
  Serial.print(F("FIFO length is: "));
  Serial.println(length, DEC);
  if (length >= MAX_FIFO_SIZE || length == 0) { // > 8M or 0
    Serial.println("Over size or size is 0.");
    return 0;
  }
  
  myCAM.CS_LOW();
  myCAM.set_fifo_burst();
  i = 0;

  while (length--) {
    temp_last = temp;
    temp =  SPI.transfer(0x00);

    // Wenn das Ende des JPEGs (Marker 0xFF 0xD9) gefunden wird...
    if ((temp == 0xD9) && (temp_last == 0xFF)) { 
      buf[i++] = temp;  // Das letzte Byte (0xD9) noch im Puffer speichern
      myCAM.CS_HIGH(); // SPI-Bus für die Kamera freigeben...
      outFile.write(buf, i); // ...und den Rest des Puffers auf die SD-Karte schreiben
      outFile.close(); // Datei schließen
      Serial.println(F("Image saved!"));
      is_header = false;
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      i = 0;
      break; // Schleife verlassen, da das Bild komplett ist.
    }

    // Wenn wir uns mitten in den Bilddaten befinden...
    if (is_header == true) {
      if (i < 256) {
        buf[i++] = temp; // Puffer füllen
      } else {
        // Puffer ist voll, also auf SD-Karte schreiben
        myCAM.CS_HIGH(); // SPI-Bus für Kamera freigeben
        outFile.write(buf, 256); // 256 Bytes schreiben
        myCAM.CS_LOW(); // SPI-Bus wieder für Kamera aktivieren
        myCAM.set_fifo_burst();
        i = 0; // Puffer-Index zurücksetzen
        buf[i++] = temp; // Das aktuelle Byte als erstes ins neue Puffer-Paket legen
      }
    } 
    
    // Wenn der Anfang des JPEGs (Marker 0xFF 0xD8) gefunden wird...
    else if ((temp == 0xD8) & (temp_last == 0xFF)) {
      is_header = true;
      myCAM.CS_HIGH();
      //Create a avi file
      k++; // Bildzähler erhöhen

      // Dateinamen dynamisch erzeugen (z.B. "/1.jpg", "/2.jpg", ...)
      sprintf(str, "/%d.jpg", k);
      Serial.print(F("Creating file: "));
      Serial.println(str);
      
      // Datei zum Schreiben öffnen
      outFile = SD.open(str, FILE_WRITE, true);

      if (!outFile) {
        Serial.println(F("File open failed"));
        while (1);
      }

      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      
      // Die ersten beiden Bytes des JPEGs in den Puffer schreiben
      buf[i++] = temp_last;
      buf[i++] = temp;
    }
  }

  myCAM.CS_HIGH();
  return 1;
}


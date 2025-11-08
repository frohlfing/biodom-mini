/**
 * Funktionstest für die ArduCAM Mini 2MP Plus (mit OV2640-Chip)
 *
 * Basierend auf https://github.com/ArduCAM/Arducam_mini/tree/main/examples/mini/ArduCAM_Mini_2MP_Plus_functions, (C)2017 Lee
 *
 * Dieser Sketch demonstriert die grundlegenden Funktionen der ArduCAM Mini 2MP Plus.
 * Er wird über die serielle Schnittstelle gesteuert. Man sendet einzelne Zeichen (Bytes), 
 * um Befehle auszulösen, wie z.B. die Auflösung zu ändern oder ein Foto aufzunehmen.
 * Die Bilddaten werden dann über die serielle Schnittstelle an den PC gesendet.
 *
 * [ArduCAM Host V2.0](https://docs.arducam.com/Arduino-SPI-camera/Legacy-SPI-camera/Software/Host-Debug-Tools/) 
 * kann diesen Datenstrom empfangen und als Bild anzeigen oder speichern.
 */

#include <Wire.h> // Für die I2C-Kommunikation. Die Kamera wird über I2C konfiguriert (Auflösung, Effekte etc.).
#include <SPI.h> // Für SPI-Kommunikation. Die Bilddaten werden schnell über SPI aus dem FIFO-Puffer gelesen.

// Im Original-Sketch sollte man die ArduCAM-Bibliothek für die Hardware anpassen, indem man in memorysaver.h das Kameramodell einkommentiert. 
// Das ist uncool, ich definiere hier direkt die Hardware und lasse die Hersteller-Bibliothek unangetastet.
//#include "memorysaver.h" // nicht aufrufen, hat ja nicht die richtige Kamera definiert
#define _MEMORYSAVER_ // Verhindert, dass memorysaver.h die Kamera-Konfiguration ändert.
#define OV2640_MINI_2MP_PLUS // Definiert das Kameramodell als OV2640 Mini 2MP Plus
#include <ArduCAM.h>  // Die Hauptbibliothek für die Steuerung der ArduCAM-Hardware.

// --- BMP-Header ---
// Ein BMP-Bild benötigt einen Header mit Informationen über Größe, Farbtiefe etc.
// Dieser Header wird vor den eigentlichen Pixeldaten gesendet, wenn ein BMP-Bild angefordert wird.
// PROGMEM speichert dieses große Array im Flash-Speicher statt im RAM, um RAM zu sparen.
#define BMPIMAGEOFFSET 66
const char bmp_header[BMPIMAGEOFFSET] PROGMEM =
{
  0x42, 0x4D, 0x36, 0x58, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x58, 0x02, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0xE0, 0x07, 0x00, 0x00, 0x1F, 0x00,
  0x00, 0x00
};

// --- Globale Variablen ---
const int CS = 17;  		//  GPIO17 des ESP32 als Chip Select Pin für die ArduCAM verwenden

bool is_header = false; 	// Hilfsvariable für den Streaming-Modus, um den Anfang eines JPEGs zu finden.
int mode = 0; 				// Steuert den aktuellen Betriebsmodus (z.B. Einzelbild, Video).
uint8_t start_capture = 0; 	// Flag, das eine neue Aufnahme auslöst.

// --- ArduCAM Objekt-Instanziierung ---
// Hier wird das Hauptobjekt für die Kamerasteuerung erstellt.
// Wir übergeben das Kameramodell (OV2640) und den CS-Pin.
ArduCAM myCAM(OV2640, CS);

// Funktions-Prototyp. Deklariert die Funktion, damit sie vor ihrer Definition aufgerufen werden kann.
uint8_t read_fifo_burst(ArduCAM myCAM);

// =======================================================================================
// SETUP-Funktion: Wird einmal beim Start des Mikrocontrollers ausgeführt
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
	Serial.println(F("ACK CMD ArduCAM Start!")); // F() speichert den String im Flash, spart RAM.
	
	// SPI-Bus initialisieren
	pinMode(CS, OUTPUT); 	// CS-Pin als Ausgang definieren.
	digitalWrite(CS, HIGH);	// CS-Pin auf HIGH setzen (inaktiv). SPI-Geräte sind aktiv, wenn CS LOW ist.
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
		if (temp != 0x55){
			// Der gelesene Wert stimmt nicht überein, die SPI-Kommunikation ist fehlerhaft!
			Serial.println(F("ACK CMD SPI interface Error!"));
			delay(1000);
			continue; // Wiederhole die Schleife nach einer Pause.
		} else {
			Serial.println(F("ACK CMD SPI interface OK."));
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
		if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
			Serial.println(F("ACK CMD Can't find OV2640 module!"));
			delay(1000);
			continue;
		} else {
			Serial.println(F("ACK CMD OV2640 detected."));
			break;
		} 
	}
	
	// --- Kamera konfigurieren ---
  	// Setze das Ausgabeformat auf JPEG. Alternativen wären z.B. BMP oder RAW.
	myCAM.set_format(JPEG);
	// Führt die grundlegende Initialisierungssequenz für die Kamera durch.
	myCAM.InitCAM();
	// Setze die Standard-Auflösung für JPEG-Bilder beim Start.
	myCAM.OV2640_set_JPEG_size(OV2640_320x240);
	delay(1000); // Eine kurze Pause geben, damit die Sensor-Einstellungen wirken können.
	
	// Setze das "Aufnahme fertig"-Flag zurück, um die Kamera in einen sauberen, definierten Zustand zu versetzen. 
	myCAM.clear_fifo_flag();
}

// =======================================================================================
// LOOP-Funktion
// =======================================================================================
void loop() {	
	uint8_t temp = 0xff, temp_last = 0;
	bool is_header = false;

	// --- Empfangen von Befehlen über die serielle Schnittstelle ---
	if (Serial.available())	{
	  	temp = Serial.read(); // Lese das empfangene Byte (den Befehl)
	  	switch (temp) {
			// --- Befehle zur Änderung der Auflösung (0x00 - 0x08) ---
			case 0:	// 160 x 120
				myCAM.OV2640_set_JPEG_size(OV2640_160x120);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_160x120"));
				break;
			case 1:	// 176 x 144
				myCAM.OV2640_set_JPEG_size(OV2640_176x144);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_176x144"));
				break;
			case 2: // 320 x 240
				myCAM.OV2640_set_JPEG_size(OV2640_320x240);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_320x240"));
				break;
			case 3:	// 352 x 288
				myCAM.OV2640_set_JPEG_size(OV2640_352x288);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_352x288"));
				break;
			case 4:	// 640 x 480
				myCAM.OV2640_set_JPEG_size(OV2640_640x480);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_640x480"));
				break;
			case 5:	// 800 x 600
				myCAM.OV2640_set_JPEG_size(OV2640_800x600);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_800x600"));
				break;
			case 6:	// 1024 x 768
				myCAM.OV2640_set_JPEG_size(OV2640_1024x768);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_1024x768"));
				break;
			case 7:	// 1280 x 1024
				myCAM.OV2640_set_JPEG_size(OV2640_1280x1024);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_1280x1024"));
				break;
			case 8:	// 1600 x 1200
				myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);delay(1000);
				Serial.println(F("ACK CMD switch to OV2640_1600x1200"));
				break;

			// --- Befehle für den Aufnahmemodus ---
			case 0x10: // Einzelnes JPEG-Bild aufnehmen
				mode = 1; // Setze den Modus auf "Single Shot"
				start_capture = 1; // Setze das Flag, um eine Aufnahme auszulösen
				Serial.println(F("ACK CMD CAM start single shoot."));
				break;
			case 0x11: // JPEG-Modus re-initialisieren
				myCAM.set_format(JPEG);
				myCAM.InitCAM();
				break;
			case 0x20: // Video-Streaming starten (kontinuierliche JPEG-Aufnahmen)
				mode = 2; // Setze den Modus auf "Video Streaming"
				start_capture = 2; // Flag zum Starten der ersten Aufnahme
				Serial.println(F("ACK CMD CAM start video streaming."));
				break;
			case 0x30: // Einzelnes BMP-Bild aufnehmen
				mode = 3;
				start_capture = 3;
				Serial.println(F("ACK CMD CAM start single shoot."));
				break;
			case 0x31: // BMP-Modus initialisieren
				myCAM.set_format(BMP);
				myCAM.InitCAM();
				// Spezifische Registereinstellungen für den BMP-Modus des Sensors
				myCAM.wrSensorReg16_8(0x3818, 0x81);
				myCAM.wrSensorReg16_8(0x3621, 0xA7);
				break;

			// --- Befehle für Bildeffekte (Weißabgleich, Sättigung, etc.) ---
      		// Die folgenden case-Blöcke rufen einfach die entsprechenden Funktionen
      		// der ArduCAM-Bibliothek auf, um Sensor-Register zu ändern.	
			case 0x40: // Lichtmodus: Auto
				myCAM.OV2640_set_Light_Mode(Auto);
				Serial.println(F("ACK CMD Set to Auto"));
				break;
			case 0x41: // Lichtmodus: Sonnig
				myCAM.OV2640_set_Light_Mode(Sunny);
				Serial.println(F("ACK CMD Set to Sunny"));
				break;
			case 0x42: // Lichtmodus: Bewölkt
				myCAM.OV2640_set_Light_Mode(Cloudy);
				Serial.println(F("ACK CMD Set to Cloudy"));
				break;
			case 0x43:	// Lichtmodus: Büro
				myCAM.OV2640_set_Light_Mode(Office);
				Serial.println(F("ACK CMD Set to Office"));
				break;
			case 0x44:	// Lichtmodus: Zuhause
				myCAM.OV2640_set_Light_Mode(Home);   
				Serial.println(F("ACK CMD Set to Home"));
				break;
			case 0x50:	// Sättigung: +2
				myCAM.OV2640_set_Color_Saturation(Saturation2); 
				Serial.println(F("ACK CMD Set to Saturation+2"));
				break;
			case 0x51:	// Sättigung: +1
				myCAM.OV2640_set_Color_Saturation(Saturation1); 
				Serial.println(F("ACK CMD Set to Saturation+1"));
				break;
			case 0x52:	// Sättigung: 0
				myCAM.OV2640_set_Color_Saturation(Saturation0); 
				Serial.println(F("ACK CMD Set to Saturation+0"));
				break;
			case 0x53:	// Sättigung: -1
				myCAM. OV2640_set_Color_Saturation(Saturation_1); 
				Serial.println(F("ACK CMD Set to Saturation-1"));
				break;
			case 0x54:	// Sättigung: -2
				myCAM.OV2640_set_Color_Saturation(Saturation_2); 
				Serial.println(F("ACK CMD Set to Saturation-2"));
				break; 
			case 0x60:	// Helligkeit: +2
				myCAM.OV2640_set_Brightness(Brightness2); 
				Serial.println(F("ACK CMD Set to Brightness+2"));
				break;
			case 0x61:	// Helligkeit: +1
				myCAM.OV2640_set_Brightness(Brightness1); 
				Serial.println(F("ACK CMD Set to Brightness+1"));
				break;
			case 0x62:	// Helligkeit: 0
				myCAM.OV2640_set_Brightness(Brightness0); 
				Serial.println(F("ACK CMD Set to Brightness+0"));
				break;
			case 0x63:	// Helligkeit: -1
				myCAM. OV2640_set_Brightness(Brightness_1); 
				Serial.println(F("ACK CMD Set to Brightness-1"));
				break;
			case 0x64:	// Helligkeit: -2
				myCAM.OV2640_set_Brightness(Brightness_2); 
				Serial.println(F("ACK CMD Set to Brightness-2"));
				break; 
			case 0x70:	// Kontrast: +2
				myCAM.OV2640_set_Contrast(Contrast2);
				Serial.println(F("ACK CMD Set to Contrast+2"));
				break; 
			case 0x71:	// Kontrast: +1
				myCAM.OV2640_set_Contrast(Contrast1);
				Serial.println(F("ACK CMD Set to Contrast+1"));
				break;
			case 0x72:	// Kontrast: 0
				myCAM.OV2640_set_Contrast(Contrast0);
				Serial.println(F("ACK CMD Set to Contrast+0"));
				break;
			case 0x73:	// Kontrast: -1
				myCAM.OV2640_set_Contrast(Contrast_1);
				Serial.println(F("ACK CMD Set to Contrast-1"));
				break;
			case 0x74:	// Kontrast: -2
				myCAM.OV2640_set_Contrast(Contrast_2);
				Serial.println(F("ACK CMD Set to Contrast-2"));
				break;
			case 0x80:	// Spezialeffekt: Antique
				myCAM.OV2640_set_Special_effects(Antique);
				Serial.println(F("ACK CMD Set to Antique"));
				break;
			case 0x81:	// Spezialeffekt: Bluish
				myCAM.OV2640_set_Special_effects(Bluish);
				Serial.println(F("ACK CMD Set to Bluish"));
				break;
			case 0x82:	// Spezialeffekt: Greenish
				myCAM.OV2640_set_Special_effects(Greenish);
				Serial.println(F("ACK CMD Set to Greenish"));
				break;  
			case 0x83:	// Spezialeffekt: Reddish
				myCAM.OV2640_set_Special_effects(Reddish);
				Serial.println(F("ACK CMD Set to Reddish"));
				break;  
			case 0x84:	// Spezialeffekt: Schwarzweiß
				myCAM.OV2640_set_Special_effects(BW);
				Serial.println(F("ACK CMD Set to BW"));
				break; 
			case 0x85:	// Spezialeffekt: Negativ
				myCAM.OV2640_set_Special_effects(Negative);
				Serial.println(F("ACK CMD Set to Negative"));
				break; 
			case 0x86:	// Spezialeffekt: Negativ Schwarzweiß
				myCAM.OV2640_set_Special_effects(BWnegative);
				Serial.println(F("ACK CMD Set to BWnegative"));
				break;   
			case 0x87:	// Spezialeffekt: Normal
				myCAM.OV2640_set_Special_effects(Normal);
				Serial.println(F("ACK CMD Set to Normal"));
				break;     
		}
	}

	// --- Zustandsmaschine basierend auf der 'mode'-Variable ---

  	// Modus 1: Einzelbildaufnahme (JPEG)
	if (mode == 1) {
		if (start_capture == 1)	{
			myCAM.flush_fifo(); 		// Leere den FIFO-Puffer von alten Daten.
			myCAM.clear_fifo_flag(); 	// Setze das "Aufnahme fertig"-Flag zurück.
			myCAM.start_capture(); 		// Löse die Aufnahme aus. Das Bild wird in den FIFO geschrieben.
			start_capture = 0; 			// Setze das Trigger-Flag zurück.
		}

		// Überprüfe kontinuierlich, ob die Aufnahme abgeschlossen ist.
		if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
			Serial.println(F("ACK CMD CAM Capture Done."));
			delay(50);
			// Rufe die Funktion auf, um die Daten aus dem FIFO zu lesen und seriell zu senden.
			read_fifo_burst(myCAM);
			// Setze das "Aufnahme fertig"-Flag zurück, damit wir auf die nächste Aufnahme warten können.
			myCAM.clear_fifo_flag();
			mode = 0; // Kehre in den Wartezustand zurück
		}
	}

	// Modus 2: Video-Streaming (kontinuierliche JPEG-Aufnahmen)
	else if (mode == 2)
	{
		while (1) { // Eine innere Endlosschleife für das Streaming
			// Prüfe, ob ein Befehl zum Stoppen (0x21) oder ein anderer Einstellungsbefehl kommt.
			temp = Serial.read();
			if (temp == 0x21) {
				start_capture = 0;
				mode = 0;
				Serial.println(F("ACK CMD CAM stop video streaming."));
				break; // Verlasse die Streaming-Schleife
			}
			
			// Hier ist der gleiche Switch-Block wie oben, um Einstellungen während des Streamings zu ändern.
			switch (temp)
			{
				case 0x40:
					myCAM.OV2640_set_Light_Mode(Auto);
					Serial.println(F("ACK CMD Set to Auto"));
					break;
				case 0x41:
					myCAM.OV2640_set_Light_Mode(Sunny);
					Serial.println(F("ACK CMD Set to Sunny"));
					break;
				case 0x42:
					myCAM.OV2640_set_Light_Mode(Cloudy);
					Serial.println(F("ACK CMD Set to Cloudy"));
					break;
				case 0x43:
					myCAM.OV2640_set_Light_Mode(Office);
					Serial.println(F("ACK CMD Set to Office"));
					break;
				case 0x44:
					myCAM.OV2640_set_Light_Mode(Home);   
					Serial.println(F("ACK CMD Set to Home"));
					break;
				case 0x50:
					myCAM.OV2640_set_Color_Saturation(Saturation2); 
					Serial.println(F("ACK CMD Set to Saturation+2"));
					break;
				case 0x51:
					myCAM.OV2640_set_Color_Saturation(Saturation1); 
					Serial.println(F("ACK CMD Set to Saturation+1"));
					break;
				case 0x52:
					myCAM.OV2640_set_Color_Saturation(Saturation0); 
					Serial.println(F("ACK CMD Set to Saturation+0"));
					break;
				case 0x53:
					myCAM. OV2640_set_Color_Saturation(Saturation_1); 
					Serial.println(F("ACK CMD Set to Saturation-1"));
					break;
				case 0x54:
					myCAM.OV2640_set_Color_Saturation(Saturation_2); 
					Serial.println(F("ACK CMD Set to Saturation-2"));
					break; 
				case 0x60:
					myCAM.OV2640_set_Brightness(Brightness2); 
					Serial.println(F("ACK CMD Set to Brightness+2"));
					break;
				case 0x61:
					myCAM.OV2640_set_Brightness(Brightness1); 
					Serial.println(F("ACK CMD Set to Brightness+1"));
					break;
				case 0x62:
					myCAM.OV2640_set_Brightness(Brightness0); 
					Serial.println(F("ACK CMD Set to Brightness+0"));
					break;
				case 0x63:
					myCAM. OV2640_set_Brightness(Brightness_1); 
					Serial.println(F("ACK CMD Set to Brightness-1"));
					break;
				case 0x64:
					myCAM.OV2640_set_Brightness(Brightness_2); 
					Serial.println(F("ACK CMD Set to Brightness-2"));
					break; 
				case 0x70:
					myCAM.OV2640_set_Contrast(Contrast2);
					Serial.println(F("ACK CMD Set to Contrast+2"));
					break; 
				case 0x71:
					myCAM.OV2640_set_Contrast(Contrast1);
					Serial.println(F("ACK CMD Set to Contrast+1"));
					break;
				case 0x72:
					myCAM.OV2640_set_Contrast(Contrast0);
					Serial.println(F("ACK CMD Set to Contrast+0"));
					break;
				case 0x73:
					myCAM.OV2640_set_Contrast(Contrast_1);
					Serial.println(F("ACK CMD Set to Contrast-1"));
					break;
				case 0x74:
					myCAM.OV2640_set_Contrast(Contrast_2);
					Serial.println(F("ACK CMD Set to Contrast-2"));
					break;
				case 0x80:
					myCAM.OV2640_set_Special_effects(Antique);
					Serial.println(F("ACK CMD Set to Antique"));
					break;
				case 0x81:
					myCAM.OV2640_set_Special_effects(Bluish);
					Serial.println(F("ACK CMD Set to Bluish"));
					break;
				case 0x82:
					myCAM.OV2640_set_Special_effects(Greenish);
					Serial.println(F("ACK CMD Set to Greenish"));
					break;  
				case 0x83:
					myCAM.OV2640_set_Special_effects(Reddish);
					Serial.println(F("ACK CMD Set to Reddish"));
					break;  
				case 0x84:
					myCAM.OV2640_set_Special_effects(BW);
					Serial.println(F("ACK CMD Set to BW"));
					break; 
				case 0x85:
					myCAM.OV2640_set_Special_effects(Negative);
					Serial.println(F("ACK CMD Set to Negative"));
					break; 
				case 0x86:
					myCAM.OV2640_set_Special_effects(BWnegative);
					Serial.println(F("ACK CMD Set to BWnegative"));
					break;   
				case 0x87:
					myCAM.OV2640_set_Special_effects(Normal);
					Serial.println(F("ACK CMD Set to Normal"));
					break;     
			}

			// Logik für die kontinuierliche Aufnahme
			if (start_capture == 2)	{
				myCAM.flush_fifo();
				myCAM.clear_fifo_flag();
				myCAM.start_capture();
				start_capture = 0; // Wird erst wieder gesetzt, nachdem das Bild gesendet wurde
			}

			// Überprüfe, ob die Aufnahme abgeschlossen ist
			if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
				uint32_t length = myCAM.read_fifo_length(); // Finde die Bildgröße heraus
				if ((length >= MAX_FIFO_SIZE) | (length == 0)) { // Fehlerprüfung
					myCAM.clear_fifo_flag();
					start_capture = 2; // Nächste Aufnahme triggern
					continue;
				}
				myCAM.CS_LOW(); // SPI-Kommunikation aktivieren
				myCAM.set_fifo_burst(); // Burst-Modus für schnelles Lesen aktivieren
				
				// --- Streaming der Bilddaten Byte für Byte ---
				temp =  SPI.transfer(0x00); // Lese erstes Byte
				length --;
				while (length--) {
					temp_last = temp;
					temp =  SPI.transfer(0x00);  // Lese nächstes Byte
					if (is_header) {
						Serial.write(temp); // Sende das Byte, wenn wir mitten im Bild sind
					}
					// Suche nach dem JPEG Start-Marker (0xFF 0xD8)
					else if ((temp == 0xD8) & (temp_last == 0xFF)) {
						is_header = true;
						Serial.println(F("ACK IMG")); // Signal an den Host: Bilddaten beginnen jetzt
						Serial.write(temp_last);
						Serial.write(temp);
					}
					// Suche nach dem JPEG End-Marker (0xFF 0xD9)
					if ((temp == 0xD9) && (temp_last == 0xFF)) {
						break; // Beende das Senden für dieses Bild
					}
					delayMicroseconds(15);
				}
				myCAM.CS_HIGH(); // SPI-Kommunikation beenden
				myCAM.clear_fifo_flag();
				start_capture = 2; // Trigger für das nächste Bild in der Videosequenz
				is_header = false;
			}
		}
	}

	// Modus 3: Einzelbildaufnahme (BMP)
	else if (mode == 3) {
		// Die Logik ist ähnlich wie bei Modus 1: Aufnahme auslösen, auf Fertigstellung warten.
		if (start_capture == 3)	{
			myCAM.flush_fifo();
			myCAM.clear_fifo_flag();
			myCAM.start_capture();
			start_capture = 0;
		}
		if (myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK)) {
			Serial.println(F("ACK CMD CAM Capture Done."));
			delay(50);

			//uint8_t temp, temp_last;
			uint32_t length = myCAM.read_fifo_length();
			if (length >= MAX_FIFO_SIZE || length == 0) { // Fehlerprüfung
				Serial.println(F("ACK CMD Over size or size is 0."));
				myCAM.clear_fifo_flag();
				return;
			}
			
			myCAM.CS_LOW();
			myCAM.set_fifo_burst();

			// --- Senden der BMP-Daten ---

			// 1. Dem Host signalisieren: "Achtung, jetzt kommt ein BMP-Header, gefolgt von Pixeldaten."
			Serial.write(0xFF); 
			Serial.write(0xAA);

      		// 2. Sende den vordefinierten BMP-Header
			for (temp = 0; temp < BMPIMAGEOFFSET; temp++) {
				Serial.write(pgm_read_byte(&bmp_header[temp]));
			}

			// 3. Lese die rohen Pixeldaten aus dem FIFO und sende sie.
      		// Die Kamera liefert RGB565-Daten (2 Bytes pro Pixel).
      		// BMP erwartet aber BGR. Die Reihenfolge der Bytes ist hier wichtig.
			for (int i = 0; i < 240; i++) { // Für jede Zeile
				for (int j = 0; j < 320; j++) { // Für jeden Pixel in der Zeile
					uint8_t VH = SPI.transfer(0x00); // Lese High-Byte des Pixels
					uint8_t VL = SPI.transfer(0x00); // Lese Low-Byte des Pixels
					Serial.write(VL); // Sende zuerst Low-Byte
					delayMicroseconds(12);
					Serial.write(VH); // Dann High-Byte
					delayMicroseconds(12);
				}
			}

			// 4. Dem Host signalisieren: "Die Übertragung der Pixeldaten ist abgeschlossen."
			Serial.write(0xBB); 
			Serial.write(0xCC);

			myCAM.CS_HIGH();
			myCAM.clear_fifo_flag();
			mode = 0; // Kehre in den Wartezustand zurück
		}
	}
}

// =======================================================================================
// Liest alle Daten aus dem FIFO und sendet sie über die serielle Schnittstelle
//
// Wird für Einzelbild-JPEG (Modus 1) verwendet.
// Arbeitsweise: 
// Die Funktion liest jedes einzelne Byte aus dem Kamera-FIFO-Puffer und sendet es sofort 
// über Serial.write() an den PC. Es findet keine Speicherung im RAM des ESP32 statt.
// =======================================================================================
uint8_t read_fifo_burst(ArduCAM myCAM) {
	uint8_t temp = 0, temp_last = 0;

	// 1. Lese die Gesamtgröße der Daten im FIFO.
	uint32_t length = myCAM.read_fifo_length();
	Serial.println(length, DEC); // Sende die Größe als Text an den PC.

	// 2. Prüfe auf ungültige Größen.
	if (length >= MAX_FIFO_SIZE || length == 0) {
    	Serial.println(F("ACK CMD Over size or size is 0."));
    	return 0;
  	}
	
	// 3. Beginne die SPI-Kommunikation.
	myCAM.CS_LOW();
	myCAM.set_fifo_burst();

	// 4. Lese und sende die Daten Byte für Byte.
  	// Diese Logik ist fast identisch zum Video-Streaming-Teil.
	temp =  SPI.transfer(0x00);
	length --;
	while (length--) {
		temp_last = temp;
		temp = SPI.transfer(0x00);
		if (is_header == true) {
			Serial.write(temp);
		} else if ((temp == 0xD8) & (temp_last == 0xFF)) { // Finde JPEG-Anfang
			is_header = true;
			Serial.println(F("ACK IMG")); // Signal an den PC: "Bilddaten folgen"
			Serial.write(temp_last);
			Serial.write(temp);
		}
		if ( (temp == 0xD9) && (temp_last == 0xFF) ) {  // Finde JPEG-Ende
			break;
		}
		delayMicroseconds(15);
	}

	// 5. Beende die SPI-Kommunikation.
	myCAM.CS_HIGH();
	is_header = false; // Zurücksetzen für das nächste Bild
	return 1;
}

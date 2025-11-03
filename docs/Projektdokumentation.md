# Projekt "Biodom Mini" – Ein Hightech-Gewächshaus

* **Datum:** 17.10.2024
* **Version:** 1.0.0
* **Autor:** [Frank Rohlfing]

Das Ziel ist ein Mini-Gewächshaus für anspruchsvolle Pflanzen, das automatisch für ein tropisches Klima sorgt.

**Features:**

- Künstliche Sonne
- Konstante Temperatur
- Konstante Luftfeuchtigkeit
- Automatische Bewässerung
- Kamera (schießt ein Bild pro Tag für Zeitraffer-Video)
- Display (zeigt aktuelle Werte an)
- Webinterface (über WLAN erreichbar)

**Inhaltsverzeichnis**

* [Projekt "Biodom Mini" – Ein Hightech-Gewächshaus](#projekt-biodom-mini--ein-hightech-gewächshaus)
  * [1. Gehäuse](#1-gehäuse)
  * [2. Steuereinheit](#2-steuereinheit)
  * [3. Sensoren](#3-sensoren)
  * [4. Aktoren](#4-aktoren)
  * [5. Sonstige Peripheriegeräte](#5-sonstige-peripheriegeräte)
  * [6. Stromversorgung](#6-stromversorgung)
  * [7. Sketche](#7-sketche-)
  * [8. Webinterface](#8-webinterface)
  * [9. Inbetriebnahme und Kalibrierung](#9-inbetriebnahme-und-kalibrierung)
  * [Anhang](#anhang-)

## 1. Gehäuse

### Gewächshaus 

AKERBÄR Gewächshaus 65, anthrazit

[Ikea](https://www.ikea.com/de/de/p/akerbaer-gewaechshaus-drinnen-draussen-anthrazit-20537175/)
29,99 €

![Gewächshaus](https://www.ikea.com/de/de/images/products/akerbaer-gewaechshaus-drinnen-draussen-anthrazit__1177014_pe895205_s5.jpg?f=xs)

![Maße Gewächshaus](https://www.ikea.com/de/de/images/products/akerbaer-gewaechshaus-drinnen-draussen-anthrazit__1357159_pe953472_s5.jpg?f=xs)

Maße: 65 x 22 (bzw. 13 oben) x 40 cm (B, T, H) 

### Blumenkasten 

Geli Blumenkasten Standard 60 cm

[Obi](https://www.obi.de/p/7331374/geli-blumenkasten-standard-ca-60-cm-anthrazit)
1,65 €

![Blumenkasten](https://bilder.obi.de/c93518e3-49c2-421c-a1b3-a229579c4aa6/prZZD/image.jpeg)

Maße: 60 x 18 x 13,7 cm (B, T, H)	

### Behälter für den Teich

Bedingungen: 
* Der Behälter muss eine grade Wand haben, um den Füllstandsensor anbringen zu können.
* Das Material darf aufgrund des Füllstandsensors nicht aus Metall sein (Glas, Kunststoff oder Keramik ist ok).
* Muss in den Blumenkasten passen (max. 12 cm tief, 13 cm hoch).

Ich habe diese Vase im Keller gefunden, passt perfekt:

* Material: Glas
* Außenmaße: 10 x 10 x 10 cm  
* Wandstärke: 4mm

### Bewässerungsset

Bewässerungsschlauch 4/6 mm (passend zur Wasserpumpe)

![Bewässerungsset](https://m.media-amazon.com/images/I/71VUWcT9Z4L._SX466_.jpg)
![Bewässerungsset](https://m.media-amazon.com/images/I/81x0UYoio2L._SX466_.jpg)

[Amazon](https://www.amazon.de/gp/product/B0DSP1KB8S)
7,98 €; "Einzelpreis": 3,99 €		

5m Schlauch, 10 Tropf-Sprinkler, 2 Ventile, Verbinder (reicht locker für 2 Häuschen)

### Elektronik-Box

Kunststoffgehäuse 200 x 120 x 75 mm

![Kunststoffgehäuse](https://m.media-amazon.com/images/I/61cdd3tspcL._SX342_.jpg)

[Amazon](https://www.amazon.de/dp/B0DJD3WC1V)
7,99 €

* IP65 (wasserdicht)

In das Gehäuse wird das Mainboard geschraubt. 

### Mainboard

Lochrasterplatine, 160 x 100 mm, blau 

![Standard-Platine](https://m.media-amazon.com/images/I/91Fkes7zXXL._SX522_.jpg)

[Amazon](https://www.amazon.de/gp/product/B081K7467B)
7,99 €

* Durchkontaktiert, runde Pads
* Rastermaß: 2,54mm
* 1,6 mm dick

### Klinkenstecker und -Buchsen für die Sensoren (Kamera ausgenommen)

3.5mm Stereo-Klinkenstecker und -Buchse 

![Klinkenstecker und -Buchse](https://m.media-amazon.com/images/I/618a1OcxYIL._SX342_.jpg)

https://www.amazon.de/dp/B09TK2HNKD
6,79 € (10 Stück)

* Muss Stereo sein (3 Pins)!

4 Klinkenbuchsen werden benötigt für:
* Luft
* Bodentemperatur
* Bodenfeuchte
* Wasserstand

**Pinbelegung:**!

![Klinkenstecker.png](assets/Klinkenstecker.png)

### Hohlstecker und -Buchen für die Aktoren und Stromversorgung

DC Power Jack Buchse und Stecker, 2.1mm, 2 Polig 

Auch DC-Stecker und DC-Buchse genannt.

![DC Buchse](https://m.media-amazon.com/images/I/71oFO08J+9L._SX342_.jpg)

[Amazon](https://www.amazon.de/dp/B07QNWSKN2)
7,49 € (12 Stück)

* Typ: 5.5 x 2.1 mm Buchse (passend zum 5.5x2.1 mm Holstecker des Netzteils)

7 Hohlstecker-Buchsen werden benötigt für:
* Lampe 1
* Lampe 2
* Heizer
* Dunst
* Pumpe
* Lüfter
* DC 12V

**Pinbelegung:**!

![Hohlstecker.png](assets/Hohlstecker.png)

## 2. Steuereinheit

### Mikrocontroller

ESP32-DevKitC V2

![ESP32-DevKitC V2](https://m.media-amazon.com/images/I/81LQ3uoqwnL._SX466_.jpg)

[Amazon](https://www.amazon.de/dp/B074RGW2VQ)
10,99 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ281_A_18-10_DE_B08BTWJGFX_e699c448-ffc6-4744-8b93-5c9f102d22b0.pdf),  
[Espressif Doku](https://docs.platformio.org/en/latest/boards/espressif32/esp32-c3-devkitc-02.html?utm_source=platformio&utm_medium=piohome),  
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/ESP_-_32_NodeMCU_Developmentboard_Datenblatt_AZ-Delivery_Vertriebs_GmbH_10f68f6c-a9bb-49c6-a825-07979441739f.pdf)


* Micro-USB-Anschluss
* 38-Pin
* ESP32-WROOM-32-Chip
* CP2102

**Anmerkung:**

Das Modell mit UCD-C macht nur Probleme. Keins meiner UCB-C-Kabel hat funktioniert.

**Pinbelegung:**
<pre>  
                                       ┌────────────────┐
                           +3.3V Out ──┤ 3V3 ESP32  GND ├── GND
                                     ──┤ EN  DevKit G23 ├── GPIO23 SPI_MOSI ──► Kamera + SD-Karte
    Reserve anlg./dgl. In ──► GPIO36 ──┤ SP  C V2   G22 ├── GPIO22 I2C_SCL  ──► Kamera + Display + Lichtstärke
         (frei anlg./dgl. In) GPIO39 ──┤ SN         TXD ├── GPIO1  (UART0_TX)
    Bodenfeuchte (analog) ──► GPIO34 ──┤ G34        RXD ├── GPIO3  (UART0_RX)
              Wasserstand ──► GPIO35 ──┤ G35        G21 ├── GPIO21 I2C_SDA  ◄─► Kamera + Display + Lichtstärke
         Dunst Relais IN7 ◄── GPIO32 ──┤ G32        GND ├── GND
         Pumpe Relais IN6 ◄── GPIO33 ──┤ G33        G19 ├── GPIO19 SPI_MISO ◄── Kamera + SD-Karte
        Lüfter Relais IN5 ◄── GPIO25 ──┤ G25        G18 ├── GPIO18 SPI_CLK  ──► Kamera + SD-Karte
        Heizer Relais IN4 ◄── GPIO26 ──┤ G26         G5 ├── ──► LED
       Lampe-2 Relais IN3 ◄── GPIO27 ──┤ G27        G17 ├── GPIO17 SPI_CS   ──► Kamera
       Lampe-1 Relais IN2 ◄── GPIO14 ──┤ G14        G16 ├── GPIO16 SPI_CS   ──► SD-Karte
                        (frei, Boot) ──┤ G12         G4 ├── GPIO4           ◄─► Bodentemperatur (One-Wire)
                                 GND ──┤ GND         G0 ├── (frei, Boot)
          Luft (One-Wire) ◄─► GPIO13 ──┤ G13         G2 ├── (frei, Boot)
                                     ──┤ SD2        G15 ├── (frei, Boot)
                                     ──┤ SD3        SD1 ├──
                                     ──┤ CMD        SD0 ├──
                              +5V in ──┤ 5V  [USB]  CLK ├──
                                       └────────────────┘
</pre>

**Sketch für Funktionstest:**

TODO

### Relais-Modul

8-Relais-Modul mit Optokoppler, 5V 

![Relais](https://m.media-amazon.com/images/I/71ODw10VbbL._SL1500_.jpg)
![Relais Anschlüsse](https://m.media-amazon.com/images/I/71N6Hp8Cw7L._SL1500_.jpg)

[Amazon](https://www.amazon.de/dp/B07CT7SLYQ)
9,59 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ041_C_9-7_DE_B07CT7SLYQ_b3af212d-3531-46ad-b4fc-e49166c8d2f2.pdf),
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/8-Relais_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH_1f00ff0d-33ca-4546-9ddc-7b2c223f5aa3.pdf)

* Max. DC:  5A, 30V pro Relais
* Maße: 50 x 140 mm

**Anmerkung:**

* Der Jumper zwischen VCC und JD-VCC muss gesteckt sein, da die Relais-Spulen auch von Steuerstromkreis versorgt werden sollen. Ein Stützkondensator wird die Induktionsspitzen beim Schalten abfangen.
* **Achtung, invertierte Logik**: Mit LOW wird das Relais geschaltet, nicht mit HIGH!

**Pinbelegung:**

**Achtung:** Die Pinbelegung sowohl im e-Book und als auch im Datasheet ist nicht korrekt abgebildet: 
NC und NO sind vertauscht. Die Symbole auf dem Modul sind korrekt, sowie hier:


<pre>
         ┌────────────────┐
         │ 8 Relay    NC1 ├──  
         │ Module    Com1 ├──
         │            NO1 ├──
         │            NC2 ├──
         │           Com2 ├── +12V        GND ─O──(schwarz)── GND
         │            NO2 ├────────────────────O──(rot)────── A1: Lampe 1
   GND ──┤ GND        NC3 ├──   
       ─►┤ IN1       Com3 ├── +12V        GND ─O──(schwarz)── GND
GPIO14 ─►┤ IN2        NO3 ├────────────────────O──(rot)────── A2: Lampe 2
GPIO27 ─►┤ IN3        NC4 ├──
GPIO26 ─►┤ IN4       Com4 ├── +12V        GND ─O──(schwarz)── GND
GPIO25 ─►┤ IN5        NO4 ├────────────────────O──(rot)────── A3: Heizer
GPIO33 ─►┤ IN6        NC5 ├──
GPIO32 ──┤ IN7       Com5 ├── +12V        GND ─O──(schwarz)── GND
       ──┤ IN8        NO5 ├────────────────────O──(rot)────── A4: Lüfter
   +5V ──┤ VCC        NC6 ├──
         │           Com6 ├── +12V         GND ─O──(schwarz)── GND
         │            NO6 ├────────────────────O──(rot)────── A5: Pumpe  
         │            NC7 ├──
         │           Com7 ├── +5V        GND ─O──(schwarz)── GND
       ──┤ GND        NO7 ├────────────────────O──(rot)────── A6: Dunst
   Jum- ┌┤ VCC        NC8 ├──  
   per  └┤ JD-VCC    Com8 ├──
         │            NO8 ├──
         └────────────────┘
</pre>

**Sketch für Funktionstest:**

TODO

## 3. Sensoren

### S1: Kamera

ArduCAM Mini 2MP Plus, OV2640  

![ArduCAM](https://cdn1.botland.de/81994-pdt_540/arducam-mini-ov2640-2mpx-1600x1200px-60fps-spi-kameramodul-fur-arduino-.jpg)
![ArduCAM Anschlüsse](https://cdn1.botland.de/81992-pdt_540/arducam-mini-ov2640-2mpx-1600x1200px-60fps-spi-kameramodul-fur-arduino-.jpg)

[BotLand](https://botland.de/kameras-fur-arduino-und-raspberry-pi/6556-arducam-mini-ov2640-2mpx-1600x1200px-60fps-spi-kameramodul-fur-arduino--5904422358242.html)
28,86 €

[User Guide](https://cdn.arducam.com/downloads/shields/ArduCAM_Mini_2MP_Camera_Shield_DS.pdf), 
[Hardware Application Note](https://www.uctronics.com/download/Amazon/ArduCAM_Mini_2MP_Camera_Shield_Hardware_Application_Note.pdf),
[Software Application Note](https://blog.arducam.com/downloads/shields/ArduCAM_Camera_Shield_Software_Application_Note.pdf)

* Sensor: OV2640
* SPI-Interface für Befehle und Kameradaten
* I2C-Interface für die Konfiguration (Adresse 0x60 für Write und 0x61 für Read)
* 3.3 - 5 V
* 20 - 70 mA 
* Größe: 34.1 x 24,4 mm
* Auflösung: 2 Megapixel, 1600 x 1200 px 
* 60 fpsm max. 8 MHz
* Output-Format: JPEG RGB

**Anmerkung:**

Ein SPI-Kabel muss kurz gehalten werden. Die mit Jumper-Kabel (Dupont) empfohlene max. Kabellänge beträgt 20 cm.
Mit geschirmtem Kabel und reduziertem Takt (1–2 MHz) sind 50 cm möglich. 

Tipps für eine stabile SPI-Verbindung:

* SCLK mit GND twisten → reduziert EM-Störungen
* MISO mit GND twisten → schützt das empfindlichste Signal
* Jede zweite Leitung auf GND bei Flachbandkabel
* Pull-Up-Widerstände an CS und MISO können helfen
* Serienwiderstand (100 Ω) direkt am Mikrocontroller-Ausgang → reduziert Reflektionen

**Warum die relative teure SPI-ArduCAM-Kamera?**

* Die meisten günstigen Kameramodule (auch die auf dem ESP32-CAM) verwenden eine DVP-Schnittstelle (Digital Video Port).
die ca. 16 GPIO-Pins belegt (also fast alle verfügbaren). Die ArduCAM belegt dagegen nur 6 Pins (4 für die SPI und 2 für I2C). 
 
**Pinout:**

| Pin | Bezeichnung | Farbe   | Beschreibung                                                                                    |
|-----|-------------|---------|-------------------------------------------------------------------------------------------------|
| 1   | CS          | schwarz | SPI Chip Select (input). Auswahl eines Geräts auf dem SPI-Bus, aktiviert durch den Low-Zustand. |  
| 2   | MOSI        | weiß    | SPI Master Output Slave Input (input). Die Ausgangsdatenleitung des SPI-Busses.                 |  
| 3   | MISO        | grau    | SPI Master Input Slave Output (output). Eingangsdatenleitung des SPI-Busses.                    | 
| 4   | SCK         | lila    | SPI Serial Clock (input). Taktleitung des SPI-Busses.                                           | 
| 5   | GND         | blau    | GND                                                                                             | 
| 6   | VCC         | grün    | 3.3V - 5V                                                                                       |  
| 7   | SDA         | gelb    | Two-Wire Serial Interface Data I/O (bidirectional). Die Ausgangsdatenleitung des I2C-Busses.    |  
| 8   | SCL         | orange  | Two-Wire Serial Interface Clock (input). Die Taktleitung des I2C-Busses.                        | 

**Pinbelegung:**
<pre>
┌────────────────┐
│ ArduCAM    SCL ├─◄───(orange)─O─── GPIO22
│ Mini 2MP   SDA ├◄─►────(gelb)─O─── GPIO21
│ Plus,      VCC ├───────(grün)─O─── +3.3V
│ OV2640     GND ├───────(blau)─O─── GND
│            SCK ├─◄─────(lila)─O─── GPIO18
│           MISO ├─►─────(grau)─O─── GPIO19
│           MOSI ├─◄─────(weiß)─O─►─ GPIO23
│             CS ├─◄──(schwarz)─O─── GPIO17 
└────────────────┘        
 </pre>

**Sketch für Funktionstest:**

**Benötigte Bibliothek:**

Header-Datei C:\Users\frank\OneDrive\Dokumente\Arduino\libraries\Arducam_mini\src\memorysaver.h modifiziert.

TODO

### S2: Luft (Raumtemperatur und Luftfeuchtigkeit)

AM2302-Modul 

Dies ist ein Temperatur- und Luftfeuchtigkeitssensor wie der DHT22, aber präziser.

![AM2302](https://m.media-amazon.com/images/I/71TPWxnUE8L._SL1500_.jpg)

[Amazon](https://www.amazon.de/dp/B078SVZB1X)
8,49 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ073_A10-7_DE_B06XF4TNT9_a2f405a2-b242-44dc-aa26-428bc637eef5.pdf?v=1721198350),
[Datasheet](https://www.makerguides.com/wp-content/uploads/2019/02/DHT22-AM2302-Datasheet.pdf)

**Anmerkung:**

* Das Protokoll für den AM2302ist (One-Wire) ist bidirektional. Demnach darf dieser Sensor nicht an die GPIO 34, 35, 36 oder 39!
* Bei einer Versorgungsspannung von 3.3V darf die Kabellänge nicht größer als 1 Meter sein.

**Pinout:**

| Pin | Bezeichnung | Farbe    | Beschreibung |
|-----|-------------|----------|--------------|
| 1   | +           | rot      | 3.3V - 5.5V  |  
| 2   | Out         | gelb     | Daten-Pin    |  
| 3   | -           | schwarz  | GND          | 

**Pinbelegung:**
<pre>
┌────────────────┐
│ AM2302       - ├──(schwarz)─O─── GND 
│ Module     Out ├─────(gelb)─O─►─ GPIO13
│              + ├─►────(rot)─O─── +3.3V
└────────────────┘               
</pre>

**Benötigte Bibliothek:**

* SimpleDHT 1.0.15 by Winlin

### S3: Bodentemperatur

DS18B20

Dies ist ein wasserdichter Temperaturfühler, kommt mit 1m-Kabel und Edelstahlhülse.

![DS18B20](https://m.media-amazon.com/images/I/71-+wN7lSdL._SL1500_.jpg)

[Amazon](https://www.amazon.de/dp/B01MZG48OE)
8,29 €	(2 Stück); Einzelpreis: 4,15 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ079_C_12-8_DE_B01MZG48OE_249c2b9f-7891-4e9a-9169-0dc281f12f11.pdf),
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/DS18B20_1mCable_datasheet.pdf)

**Anmerkung:**

Das Protokoll für den DS18B20 (One-Wire) ist bidirektional. Demnach darf dieser Sensor nicht an die GPIO 34, 35, 36 oder 39!

**Pinout:**

| Pin  | Bezeichnung | Farbe   | Beschreibung |
|------|-------------|---------|--------------|
| 1    | VCC         | rot     | 3.3 - 5.5V   |  
| 2    | DATA        | gelb    | Daten-Pin    | 
| 3    | GND         | schwarz | GND          |

**Pull-Up-Widerstand R1 = 4.7 kΩ** 

Wird zwischen dem roten Kabel (VCC) und dem gelben Kabel (DATA) an die Buchsenleiste gelötet.

**Pinbelegung mit Pullup-Widerstand:**
<pre>
┌────────────────┐                   R1=4.7k
│ DS18B20    GND ├──(schwarz)─O────────────────── GND 
│           DATA ├─────(gelb)─O─────┬───────────► GPIO4
│            VCC ├─►────(rot)─O──┐  └─[ R1 ]─┐
└────────────────┘               └───────────┴─── +3.3V
</pre>

**Benötigte Bibliothek:**

* OneWire 2.3.9 by Paul Stoffregen
* DallasTemperature 4.0.5 by Miles Burton

### S4: Bodenfeuchte

Kapazitives Hygrometer-Modul V1.2, analog

Dies ist ein kapazitiver Bodenfeuchtigkeitssensor mit analogem Ausgang.

Auch "Capacitive Soil Moisture Sensor" genannt. 

![Hygrometer](https://m.media-amazon.com/images/I/61f-Y-vFplL._SX522_.jpg)

[Amazon](https://www.amazon.de/dp/B07HJ6N1S4)
6,46 €

[Praxisbericht](https://www.makerguides.com/capacitive-soil-moisture-sensor-with-arduino/), 
[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ063_A_16-13_DE_B07HJ6N1S4_1746d1f6-6b82-4b8a-bee1-88d83344cb5a.pdf),
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/Hygrometer_V1.2_Sensor_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf)

* 5V (!)
* Größe: 22 x 97 x 9 mm  

**Anmerkung:** 

[Paradisetronic.com](https://www.amazon.de/dp/B08944FNXV) bietet den Sensor für 3.3V an. Dazu müsste aber ein TLC555C oder TLC555I verbaut sein. 
Tatsächlich ist wie bei AZDelivery ein NE555 drauf (minimale Versorgungsspannung von 4.5V lt. Datenblatt). 
Damit ist der Betrieb mit 3.3V nicht möglich oder zumindest kritisch (s. Praxisbericht oder [Kundenrezension](https://www.amazon.de/gp/customer-reviews/R2E7NX4GYVWY0/ref=cm_cr_getr_d_rvw_ttl?ie=UTF8)). 

**Pinout:**

| Pin  | Bezeichnung | Farbe   | Beschreibung |
|------|-------------|---------|--------------|
| 1    | GND         | schwarz | GND          |  
| 2    | VCC         | rot     | 5V           | 
| 3    | AOUT        | gelb    | Daten-Pin    |

**Anmerkung:**

Der Sensor hat eine Betriebsspannung von 5 V. Die maximale Ausgangsspannung liegt aber deutlich unter 3.3 V. Ein Spannungsteiler ist also nicht erforderlich.

**Pinbelegung:**
<pre>
┌────────────────┐
│ Capacitve  GND ├──(schwarz)─O─── GND 
│ Hygrometer VCC ├──────(rot)─O─── +5V
│ v1.2      AOUT ├─►───(gelb)─O─►─ GPIO34
└────────────────┘
</pre>   

**Messbereich:**

Darüber steht nichts im Datenblatt. Die Werte habe ich experimental ermittelt.

* Komplett trocken (in der Luft): 2080 mV
* Komplett feucht (im Wasser): 800 mV 

**Sketch für Funktionstest:**

**Benötigte Bibliothek:**

TODO

**Kalibrierung:**

* Weiße Markierung für die Einstecktiefe beachten!

Messung der Rohwerte: 
```
void setup() {
  Serial.begin(115200);  
}

int valMax = 0;
int valMin = 99999; 

void loop() {
  int val = analogRead(34);  
  if (val < valMin) {
    valMin = val;
  }
  if (val > valMax) {
    valMax = val;
  }
  Serial.print("Min:");
  Serial.print(valMin);
  Serial.print(",Max:");
  Serial.print(valMax);
  Serial.print(",Curr:");
  Serial.println(val);
  delay(1000);
}
``` 
 
Die Messung ergibt 
* im Wasser: 995 bis 1055 => ca. unter 1100
* in der Luft: 2642 bis 2734  => ca. über 2500

Der folgende Code verwendet die Rohwerte und ordnet sie einem Bereich von 0 bis 100% zu:

```
const int valWater = 1100;
const int valAir = 2500;

void setup() {
  Serial.begin(115200);
}

void loop() {
  int val = analogRead(34);
  int percent = map(val, valAir, valWater, 0, 100);
  percent = constrain(percent, 0, 100);
  Serial.println(percent);
  delay(1000);
}
```

### S5: Wasserstand

XKC-Y25-NPN 

Dies ist ein berührungsloser Füllstandsensor.

![XKC-Y25-NPN](https://m.media-amazon.com/images/I/61vWOl7DAtL._AC_SL1001_.jpg)

[Amazon](https://www.amazon.de/dp/B0C1ZVZF2L)
10,19 €

[Tutorial](https://www.kreativekiste.de/arduino-anschluss-fluessigkeitsstandsensor-xkc-y25-pnp-sps), 
[Datasheet](https://www.nuggetforum.de/media/kunena/attachments/23214/NPN-bb00ea11-07b9-4b43-be78-ce18f0e40b48.pdf)
 
* 5-12 V, 100 mA
* berührungslos, kann Kunststoff-, Glas-, Keramik- und andere nichtmetallische Behälter durchdringen (≤ 20 mm dick)
* wasserdicht
* Ausgang: NPN, digital (LOW bei Wassererkennung)
* Kabellänge: ca. 50 cm
* Pegelfehler: ±1,5 mm

**Pinout:**

| Pin | Bezeichnung | Farbe am Sensor | Farbe am Klinkenstecker | Beschreibung                             |
|-----|-------------|-----------------|-------------------------|------------------------------------------|
| 1   | VCC         | braun           | rot                     | Versorgung (5–12V)                       |  
| 2   | OUT         | gelb            | grün                    | Digitaler Ausgang                        | 
| 3   | GND         | blau            | weiß                    | GND                                      |
| 4   | MODE        | schwarz         | -                       | Modus-Umschaltung (wird nicht benötigt)  | 

**Anmerkung:**

* Eventuell sind Schwarz und Gelb vertauscht (OUT = Schwarz, MODE = Gelb)

TODO sicherstellen, dass die Farben richtig dokumentiert sind.

Pegel am Ausgang:

* Der Sensor wird mit 5 V versorgt, aber das ist in diesem Fall kein Problem für den ESP32. 
	Denn am Ausgang liegen niemals die +5V an: 
	* Wasser erkannt: Er verbindet seinen Ausgang (OUT) mit GND.
	* Kein Wasser: Der Ausgang ist hochohmig (existiert quasi nicht).

**Pull-Up-Widerstand R2 = 10 kΩ**

Wird zwischen +3.3V vom ESP32 und dem gelben Kabel (OUT) an die Buchsenleiste gelötet.

So wird der Pegel auf HIGH (+3.3V) gezogen, wenn Trockenheit erkannt wird.    

**Pinbelegung mit Pullup-Widerstand:**
<pre>
┌────────────────┐                     R2=10k 
│ XKC-Y25    VCC ├────(braun)─O───────────────── +5V   
│ -NPN       OUT ├─►───(gelb)─O─────┬──[ R2 ]─── +3.3V 
│            GND ├─────(blau)─O───┐ └─────────►─ GPIO35    
│           MODE ├──(schwarz)─O   └───────────── GND         
└────────────────┘    
</pre>

**Sketch für Funktionstest:**

TODO

### S6: Lichtstärke

GY-302 BH1750 Lichtsensormodul

Dieser Sensor ist sehr genau und misst das Licht direkt in LUX.

![GY-302 BH1750](https://m.media-amazon.com/images/I/713WSoL6ERL._SX522_.jpg)

[Amazon](https://www.amazon.de/dp/B07TKWNGZ4)
5,49 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ104_A6-9_DE_B07TKWNGZ4.pdf)
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/GY-302_Licht_Sensor_Modul_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf)

* I2C-Schnittstelle (Adresse 0x23 (oder 0x5C, wenn der ADDR-Pin auf HIGH gelegt wird))
* Der IC auf der Platine direkt über dem Schriftzug "BH1750" ist das lichtempfindliche Bauteil. 
**Pinout:**

| Pin | Bezeichnung | Farbe   | Beschreibung                    |
|-----|-------------|---------|---------------------------------|
| 1   | VCC         | rot     | 3.3V, 5V                        |  
| 2   | GND         | schwarz | GND                             | 
| 3   | SCL         | braun   | I2C Serial Clock (input)        | 
| 4   | SDA         | orange  | I2C Serial Data (bidirectional) |  
| 5   | ADDR        | gelb    | Geräteadresse (input)           |  

**Pinbelegung:**
<pre>
┌────────────────┐
│ GY-302     VDD ├──────(rot)─O── +3.3V
│ BH1750     GND ├──(schwarz)─O── GND
│            SCL ├────(braun)─O─◄─ GPIO22
│            SDA ├───(orange)─O◄─► GPIO21 
│           ADDR ├─────(gelb)─O──
└────────────────┘
</pre>

**Anmerkung:**

Da kein anderes I2C-Gerät die Adresse 0x23 verwenden, wird ADDR nicht benötigt.

**Sketch für Funktionstest:**

TODO

### S7: Reserve

Der Analog-/Digitaleingang GPIO36 wird über eine Stiftleiste zugänglich gemacht. Er ist noch nicht verplant.

Als Beispiel könnte ein Fotowiderstand (auch Fotozelle oder LDR (Licht-Dependent Resistor) genannt) angeschlossen werden:

LDR5528

[Amazon](https://www.amazon.de/dp/B089YNCYG4)
4,49 € (10 Stück), Einzelpreis: 0,49 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ098_B_16-1_DE_B089YNCYG4_ff124573-82f8-46ba-8411-8660b6462697.pdf)
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/Fotowiderstand_Photo_Resistor_Dioden_150V_5mm_LDR5528_GL5528_5528_Datenblatt_AZ-Delivery_Vertriebs_GmbH.pdf)

5mm
* 0.5 - 200 kΩ
* Maximale Spannung: 150 V
* Maximale Leistung: 100 mW

Der Fotowiderstand ist lichtabhängig. Je mehr Licht auf die Oberfläche fällt, desto geringer ist sein Widerstand.

Der Sensor wird zusammen mit einem Widerstand als Spannungsteiler an einem analogen Eingang des ESP32 geschaltet. 
Dieser wandelt die Spannung in ganzzahlige Werte zw. 0 (dunkel) und 4049 (hell) um, die als ADC-Wert oder Analogwert bezeichnet werden.

<pre>
                    ↙↙
GND ───[ R3 ]──┬──[ LDR ]──── +3.3V
        10 kΩ  │  LDR5528
               └───────────►─ GPIO36
</pre>

**Sketch für Funktionstest:**

## 4. Aktoren

### A1, A2: Lampe

LED Grow Light 12V/60W, Vollspektrum, 4:1, IP65

![LED-Streifen](https://m.media-amazon.com/images/I/81TPLiD5MnL._AC_SX679_.jpg)

[Amazon](https://www.amazon.de/gp/product/B076D3GCH4)
18,99 €

* 12V, 60W
* Rot:Blau = 4:1
* Vollspektrum
* IP65 (wasserdicht)
* 5m-Streifen (1cm breit) mit 300 LEDs (SMD 5050)
* Hersteller: Tesfish

12V/60W = 5A (lt. Hersteller) -> Damit ist das Limit für ein Relais des Relais-Modul erreicht. Daher teile ich den 
5m-Streifen in zwei Hälften, um die Last auf zwei Relais verteilen zu können. 
Außerdem könnte ich das Licht so in zwei Helligkeitsstufen schalten.  

**Aufteilung des LED-Streifens in 2 Lampen:**
<pre>
            O─▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓─O┐
          ┌─O─░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░─O│┐
          │┌O─▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓─O┘│
Lampe 1 O─┼─O─░░░░░░░░░░░░░░░  ░░░░░░░░░░░░░░─O─┘
Lampe 2 O─│┼O─▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓─O┐
          └─O─░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░─O│┐
           └O─▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓─O┘│
            O─░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░─O─┘
            <-------------- 60cm ------------->
</pre>

**UPDATE: Tatsächliche Leistung (Messwerte):**

* Leistung pro Streifenhälfte (2,5m): Spannung: 12 V, Strom: 2 A -> Reale Leistung: P = U * I = 12 V * 2 A = 24 Watt
* Leistung des gesamten Streifens (2 x 2,5m): Spannung: 11.9 V, Strom: 3.8 A -> Reale Gesamtleistung: P = 11.9 V * 3.8 A ≈ 45 Watt

Der gemessene Strom ist unter 4A, es würde 1 Relais reichen. (Sicherheitsmarge von 20% ist ok). 
Aber dann kann ich nicht mehr zwei Helligkeitsstufen schalten. 

**Pinbelegung:**
<pre>
┌────────────────┐
│ LED Grow  +12V ├──────(rot)─O── Relais IN2
│ Light 1        │
│ 30W        GND ├──(schwarz)─O── GND
└────────────────┘
┌────────────────┐
│ LED Grow  +12V ├──────(rot)─O── Relais IN3
│ Light 2        │
│ 30W        GND ├──(schwarz)─O── GND
└────────────────┘
</pre>

**Passt die Lampe zum Gehäuse?**

**Ideale Lichtmenge (PPFD):**

* Die Lichtmenge wird in der Pflanzenwissenschaft in PPFD (Photosynthetic Photon Flux Density) gemessen. 
* Die Einheit ist µmol/m²/s. Der Wert gibt an, wie viele Photonen pro Sekunde auf einen Quadratmeter treffen. 
* Typische PPFD-Werte:
	* Cannabis:
		* Keimlinge/Stecklinge: 150 - 300 µmol/m²/s
		* Vegetatives Wachstum: 300 - 600 µmol/m²/s
		* Blütephase: 600 - 1.000 µmol/m²/s
	* Zitrone, Mango, Avocado: 
		* Wachstum (Jungpflanze): 300 - 500 µmol/m²/s
		* Fruchtbildung: > 700 µmol/m²/s.
	* Allround-Gewächshaus:
		* ≈ 400-600 µmol/m²/s

**Die Lichtmenge der Lampe:**

* **Leistung der Lampe:** Die Lampe wird mit 60W beworben. Laut [Kundenrezension](https://www.amazon.de/gp/customer-reviews/RP3TF4WHOI91K/ref=cm_cr_dp_d_rvw_ttl?ie=UTF8) leistet die Lampe nur 40 W. 

* **Effizienz der Lampe:** Die Effizienz einer Lampe wird in µmol/Joule angegeben. Sie sagt aus, wie viele Licht-Photonen (µmol) pro Wattsekunde (Joule) erzeugt werden.
Typische Werte für SMD 5050 Grow-LEDs, aus denen die Lampe besteht, liegen bei 1,5 bis 1,8 µmol/Joule. 

* **Pflanzenfläche:** Das AKERBÄR Gewächshaus hat unten eine Fläche von 0,60 x 0,2 m = 0,12 m².

* Jetzt können wir rechnen:

	* Gesamter Photonenfluss (PPF): Wie viele Photonen produziert die 40W-Lampe insgesamt pro Sekunde?
  
		PPF (µmol/s) = Leistung (Watt) * Effizienz (µmol/J)
	
		* Min: 40 W * 1.5 µmol/J = 60 µmol/s
		* Max: 40 W * 1.8 µmol/J = 72 µmol/s		

	* PPFD auf der Fläche - Jetzt verteilen wir diese Gesamtmenge an Photonen auf unsere Anbaufläche.
  
		PPFD (µmol/m²/s) = PPF (µmol/s) / Fläche (m²)

		* Min: 60 µmol/s / 0,12 m² = 500 µmol/m²/s
		* Max: 72 µmol/s / 0,12 m² = 600 µmol/m²/s

=> Die Lampe passt perfekt zum Gehäuse!

### A3: Heizer

Silikon-Heizmatte, 12V/12W, 60°C

![Heizmatte](https://m.media-amazon.com/images/I/51+UWx+HLdL._AC_SX679_.jpg)

[Amazon](https://www.amazon.de/dp/B09VB1Q2G4)
7,19 €

* 12V, 12W
* bis 60°C
* 10 x 12 x 1,5cm

Der tatsächliche Strombedarf beträgt 930 mA (gemessen).

**Pinbelegung:**
<pre>
┌────────────────┐
│ Heating   +12V ├─────O── Relais IN4 
│ max. 60°C      │
│ 12W        GND ├─────O── GND
└────────────────┘
</pre>


### A4: Lüfter

PC-Lüfter, Silent, 12V, 8.5 CFM, 50 mm

![Super Silent Fan](https://m.media-amazon.com/images/I/61AklCsOTPL._SX466_.jpg)

[Amazon](https://www.amazon.de/dp/B01HTN8XZE)
6,94 €;

* 12 V, 50 mA
* 50 x 50 mm
* 8,5 CFM
* 17,5 dB
* 3500 Umdrehungen pro Minute
 
**Pinout:**

| Pin | Bezeichnung | Farbe   | Beschreibung |
|-----|-------------|---------|--------------|
| 1   | +12V        | rot     | Versorgung   |  
| 2   | GND         | schwarz | GND          |  
| 3   | RPM         | gelb    | Tachosignal  | 

**Anmerkung:**

Das gelbe Kabel mit der Bezeichnung "RPM" (Revolutions Per Minute) wird nicht benötigt. 
Es liefert ein "Tachosignal" (typischerweise 2 Pulse pro Umdrehung). Man könnte damit z.B. per Mikrocontroller prüfen, ob der Lüfter läuft oder blockiert ist.

**Pinbelegung:**
<pre>
┌────────────────┐
│ Fan       +12V ├──────(rot)─O── Relais IN5 
│                │
│ 0.6W       GND ├──(schwarz)─O── GND
└────────────────┘
</pre>

**Montage des Lüfters:**

[Montageanleitung des PC-Lüfters](https://m.media-amazon.com/images/I/91Py5fJkHrL.pdf)

Pflanzen verbrauchen CO₂ und geben Sauerstoff ab. Die "verbrauchte" und feuchte Luft steigt auf und sammelt sich im oberen Bereich des Gewächshauses.
Die Luft muss also von unten nach oben zirkulieren. 

Der Lüfter wird die Luft nach außen pusten und muss daher oben angebracht werden.

**Reicht die Leistung des Lüfters aus?**

Wie schnell der gewählte Lüfter die gesamte Luft im Gewächshaus austauschen kann. 
Ein guter Wert für ein kleines Gewächshaus liegt bei einer Luftaustauschrate von mindestens 1-mal pro Minute.

* Der Luftstrom der Lüfter wird in CFM (Cubic Feet per Minute) angegeben: 1 CFM ≈ 0,0283 m³/min

* Der ausgewählte PC-Lüfter pustet 8,5 CFM = 8.5 * 0,0283 m³/min = 0,24055 m³/min

* Das AKERBÄR Gewächshaus hat ein Volumen von 0,65 m * (0,22 m + 0,13 m)/2 * 0,40 m = 0,0455 m³.

* Die Luftaustauschrate ist somit 0,24055 m³/min / 0,0455 m³ ≈ **5,3-mal pro Minute**

=> Der Lüfter reicht völlig aus.

### A5: Pumpe

Mini 370 Membranpumpe, 12V

![Wasserpumpe](https://m.media-amazon.com/images/I/51t9cPjFAmL._SX466_.jpg)
![Wasserpumpe](https://ae01.alicdn.com/kf/S3b09269c385e4baa86236b79689a706dc.jpg)

[Amazon](https://www.amazon.de/dp/B0DX5PVTBQ)
8,98 €

Wird mit 1m Schlauch geliefert (5 mm innen).

* max. 300mA
* Saughöhe: 2m
* Durchflussmenge: 700ml

**Pinbelegung:**
<pre>
┌────────────────┐
│ Mini      +12V ├──────(rot)─O── Relais IN6 
│ Pump           │
│ 3.6W       GND ├──(schwarz)─O── GND
└────────────────┘
</pre>

### A6: Dunst

Ultraschall-Vernebler, 5V/5W

![Vernebler](https://m.media-amazon.com/images/I/71PqBVnt2bL._AC_SL1500_.jpg)
![Vernebler Maße](https://m.media-amazon.com/images/I/61kI86Imn5L._AC_SX425_.jpg)

[Amazon](https://www.amazon.de/gp/product/B0FM2FZD4Z)
12,49 €		

* 5V, 5W
* 1,4m Kabel
* 4,7 x 2,5 cm (B, H)
* Wassermangelschutz
* LED Farbwechsel

USB-Stecker abknipsen und Klinkenstecker anlöten.
Kabeladern:
* Braune = VCC
* Weiß = GND

**Pinbelegung:**
<pre>
┌────────────────┐
│ Mist       +5V ├──────(rot)─O── Relais IN7 
│ Maker       D- ├─────(weiß)─O── 
│ 5W          D+ ├─────(grün)─O── 
│            GND ├──(schwarz)─O── GND
└────────────────┘
</pre>

## 5. Sonstige Peripheriegeräte

### Z1: Display

1.3 Zoll OLED Display, SSH1106

![Display](https://m.media-amazon.com/images/I/71HZLmSHn8L.jpg)

[Amazon](https://www.amazon.de/dp/B078J78R45)
8,49 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ003_A_1-6_DE_B078J78R45_1.pdf?v=1721030497), 
[Datasheet](https://cdn.shopify.com/s/files/1/1509/1638/files/1_3_Zoll_Display_Datenblatt_AZ-Delivery_Vertriebs_GmbH_rev.pdf?v=1606164520)

* 3.3V, 5V
* I2C-Schnittstelle (Adresse 0x3C)
* 128 x 64 Pixel
* Chip: SSH1106 
* Platine: 36 x 34 mm

**Pinout:**

| Pin | Bezeichnung | Farbe | Beschreibung                    |
|-----|-------------|-------|---------------------------------|
| 1   | VDD         | grün  | 3.3V, 5V                        |  
| 2   | GND         | blau  | GND                             | 
| 3   | SCK         | lila  | I2C Serial Clock (input)        | 
| 4   | SDA         | grau  | I2C Serial Data (bidirectional) |  

**Pinbelegung:**
<pre>
┌────────────────┐
│ 1.3″       VDD ├──────(rot)─O── +3.3V
│ OLED       GND ├──(schwarz)─O── GND
│ Display    SCK ├────(braun)─O─◄─ GPIO22
│ SSH1106    SDA ├───(orange)─O◄─► GPIO21 
└────────────────┘
</pre>

**Sketch für Funktionstest:**

TODO

### Z2: Leuchtdiode

Nützlich zum Debuggen

Diode, 3mm, rot

Das längere Beinchen ist der Pluspol.

**Pinbelegung mit Vorwiderstand:**

<pre>
                      ↗↗     
GPIO5 ─▶───[ R2 ]─────▶├───── GND
            330 Ω    LED1
</pre>

**Sketch für Funktionstest:**

TODO

### M1: SD-Kartenleser

MicroSD SPI Kartenleser, 3.3V 

Dies ist ein MicroSD-Kartenlesemodul.

![Kartenleser](https://m.media-amazon.com/images/I/61vfbUm6CNL._SL1200_.jpg)
![Kartenleser Pins](https://m.media-amazon.com/images/I/61Ul-PLkYDL._SX466_.jpg)

[Amazon](https://www.amazon.de/gp/product/B0DQZFBCST)
4,29 € (3 Stück); Einzelpreis: 1,43 €

Auf der Amazon-Webseite ist die Versorgungsspannung falsch angegeben. Richtig ist 3.3V.  

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ234_C_10-4_DE_B06X1DX5WS_ec398a85-2534-4b15-9f3e-1d3a37b6ce96.pdf?v=1721122338), 

* 3.3V
* SPI-Schnittstelle
* 0,2 bis 200 mA
* gerade Pins!
* Platinengröße: 17,9 x 17,9 mm

**Pinout:**

| Pin | Bezeichnung | Beschreibung                           |
|-----|-------------|----------------------------------------|
| 1   | 3v3         | 3.3V                                   |  
| 2   | CS          | SPI Chip Select (input)                |  
| 3   | MOSI        | SPI Master Output Slave Input (input)  |  
| 4   | CLK         | SPI Serial Clock (input)               | 
| 5   | MISO        | SPI Master Input Slave Output (output) | 
| 6   | GND         | GND                                    | 

**Pinbelegung:**
<pre>
┌────────────────┐
│ MicroSD    GND ├───O─── GND
│ SPI       MISO ├───O─►─ GPIO19
│ Module     CLK ├───O─◄─ GPIO18
│           MOSI ├───O─◄─ GPIO23 
│             CS ├───O─◄─ GPIO16 
│            3v3 ├───O─── +3.3V
└────────────────┘
 </pre>

**Sketch für Funktionstest:**

TODO

**Speicherkarte:**

SanDisk Ultra 32 GB microSDHC, Class 10, U1

![Speicherkarte](https://m.media-amazon.com/images/I/51+lCdUUPoL._AC_SX425_.jpg)

[Amazon](https://www.amazon.de/dp/B08HVWJM8C)
7,78 €

* MicroSD (bis 2 GB) und MicroSDHC (4 bis 32 GB) funktionieren mit SPI.
* Geschwindigkeitsklasse: C10 ist Standard für Arduino/ESP32-Projekte (Schreibgeschwindigkeit: mind. 10 MB/s)
* Bus-Typ: UHS-I (U1 oder U3)
* Hersteller: SanDisk (Kingston oder Transcend wäre auch ok)

Die Karte sollte FAT32-formatiert sein.

**4 GB SD-Karte Class 4. vs. 32 GB HC Class 10**

Ich habe noch eine alte 4 GB SD-Karte Class 4. Wie lang reicht die Karte, wenn pro Stunde ein Bild geschossen wird? 
Wie schnell ist sie in der Praxis?

* 93% einer SD-Karte sind durchschnittlich praktisch nutzbar.
* Die höchste Auflösung der ArduCAM-Kamera ist  1600 x 1200 Pixel. Die Bilder werden im JPEG-Format gespeichert. Ich schätze 600 KB durchschnittlich.  

Es folgt:
* 4 GB: 4 GB * 0,93 * 1024 * 1024 / 600 KB = ≈ 6500 Bilder => 6500 / 24  ≈ **270 Tage**
* 32 GB: 32 GB * 0,93 * 1024 * 1024 / 600 KB = ≈ 52000 Bilder => 52000 / 24 / 365 ≈ **6 Jahre**

Theoretische Schreibgeschwindigkeit pro Bild:
* Class 4 (min. 4 MB/s): 0.6 MB / 4 MB/s = 150 ms. 
* Class 10 (min. 14 MB/s): 0.6 MB / 10 MB/s = 60 ms.

In der Realität ist der Prozess durch den ESP32 und die SPI-Kommunikation limitiert. 
Der gesamte Vorgang (Auslesen von der Kamera + Schreiben auf die Karte) wird vermutlich eher bei 1 bis 1,5 Sekunden liegen.

Ich werde das mal nachmessen.
 
**Sketch für Funktionstest:**   

## 6. Stromversorgung

### P1: Spannungswandler 12V zu 5V

Step-Down-Module XL4015, 5A

![Step-Down-Modul](https://m.media-amazon.com/images/I/61Miv6WUVJL._SX385_.jpg)

[Amazon](https://www.amazon.de/dp/B07VQCXDTC)
9,99 €	(3 Stück); Einzelpreis:	3,33 €

[AZ-Delivery e-Book](https://cdn.shopify.com/s/files/1/1509/1638/files/AZ233_C_9-4_DE_B07SRXR1VT_7e75912c-950c-49d9-aeb2-2b6e195245f1.pdf?v=1721042154),
[Datasheet](https://www.xlsemi.com/datasheet/XL4015-EN.pdf)

**Übersetzungsfehler im eBook:** "Einstellbare Eingangsspannung" muss heißen "Einstellbare Ausgangsspannung" (orig.: "Output Voltage is Adjustable")
 
* von 8V-36V zu 1,25V-32V einstellbar, max. 5A
* Arbeitstemperatur: -40 ℃ bis +85 ℃
* Übertemperaturschutz (thermische Abschaltfunktion)
* Größe: 54 x 23 x 18 mm (L x B x H)
* XL4015-Chip

**Pinbelegung:**
<pre>
                   ┌────────────────┐
Netzteil DC +12V ──┤ IN+       OUT+ ├──┬──────────────────── ESP32 +5V
                   │  XL4015 Module │  └─⁺▌|─┐ C1: 470µF
             GND ──┤ IN.       OUT- ├────────┴────────────── GND
                   └────────────────┘
</pre>

**Stützkondensator C1**:

C1: Elko 470µF, 16V

Generell ist ein Stützkondensator zu empfehlen, dieser schützt vor Spannungseinbrüchen. 
Erst recht, wenn ein Relais eingesetzt wird, dessen JD-VCC-Jumper gesteckt ist.

Wird so nah wie möglich am Ausgang des Spannungswandlers gelötet.
Das längere Beinchen und der Pin ohne Markierung auf dem Gehäuse ist PLUS (+).
 
**Anmerkung:**

In- und Out- ist dasselbe Potenzial. 

Die Ausgangsspannung muss auf **5 Volt** eingestellt werden!

### Netzteil

12V Steckernetzteil Adapter, 120W

![Netzteil](https://m.media-amazon.com/images/I/71J0iCEh9IL._SX466_.jpg)

[Amazon](https://www.amazon.de/gp/product/B0DBL4FV6C)
11,99 €

5.5x2.1 mm DC-Holstecker

**Benötigte Leistung / max. Stromaufnahme:**

5V-Komponenten (über Spannungswandler):

* ESP32-DevKitC C: 0,2 – 0,3A (je nach WLAN-Last)
* ArduCAM (SPI): ca. 0,1A (bei Kameraaufnahme)
* SD-Kartenleser (SPI): ca. 0,05A (nur bei Zugriff)
* OLED Display: ca. 0,02A (sehr gering)
* AM2302-Modul: ca. 0,002A (vernachlässigbar)
* DS18B20: ca. 0,002A (vernachlässigbar)
* Hygrometer-Modul: ca. 0,005–0,01 A (gering)
* XKC-Y25-NPN: ca. 0,1 A (bei Aktivierung)
* Ultraschall-Vernebler: ca. 1,0 A (Hauptlast auf 5V)

=> Summe worst case: ca. 1,6 A für 5V-Komponenten

Der Spannungswandler liefert bis zu 5A, also reichlich Reserve.

12V-Komponenten:

* LED Grow Light (5m, 60W): ca. 60W → 5,0A (Hauptlast, Dauerbetrieb)
* Heizmatte (12W):	12W → 1,0A (Dauerlast)
* 2x PC-Lüfter: ca. 0,1A gesamt
* Wasserpumpe Mini: max. 0,3A (Kurzzeitbetrieb)
* Relais-Modul (8-Kanal, 6 im Gebrauch): ca. 0,3A (50mA pro Relais)	(nur wenn alle Relais gleichzeitig anziehen)

=> Summe worst case: ca. 6,7 A für 12V-Komponenten

Max. Gesamtlast am Netzteil: **ca. 8,3 A**

Das 12V-Netzteil liefert 10 A, also sind ca. 1,7 A Reserve. Das ist knapp, aber im grünen Bereich.
 
## 7. Schaltplan

Den Sachaltplan habe ich mit KiCad 9.0 erstellt.

![Schaltplan.png](assets/Schaltplan.png)

### Leiterplatten-Entwurf

![Leiterplatte.png](assets/Leiterplatte.png)

## 8. Sketche 

Arduino-Programmskript, .ino-Datei

### Steuerungslogik

*   **Lampe 1 (IN1):** Rein zeitgesteuert (z.B. 16 Std. an, 8 Std. aus).
*   **Lampe 2 (IN2):** Rein zeitgesteuert (z.B. 16 Std. an, 8 Std. aus).
*   **Heizer (IN3):** Gesteuert von **Bodentemperatur**. Hält die Erde auf Solltemperatur.
*   **Pumpe (IN4):** Gesteuert von **Bodenfeuchte** und **Wasserstand**. Bewässert nur, wenn Boden zu trocken UND genug Wasser im Teich ist.
*   **Dunst (IN5):** Gesteuert von **Luftfeuchtigkeit** und **Wasserstand**. Befeuchtet nur, wenn Luft zu trocken UND genug Wasser im Teich ist.
*   **Lüfter (IN6):** Läuft an, wenn **Lufttemperatur** oder **Luftfeuchtigkeit** die Maximalwerte überschreiten.

### Konfiguration (Globale Variablen)

*Sollwerte: TODO
*Pins: TODO
*WLAN-Daten: TODO

### Programmierung der Sketch

#### Serielle Kommunikation

Kommunikation starten (Serial.begin(115200)).

#### WLAN

Mit dem WLAN verbinden.

#### Relais

* Pin-Modi für die Relais auf OUTPUT setzen.
* Testlauf: Lade einen einfachen Test-Code hoch, der nacheinander jedes Relais für 5 Sekunden schaltet. Überprüfe, ob Licht, Pumpe, Heizer etc. korrekt reagieren.

#### Display 

Display initialisieren und einen Startbildschirm anzeigen.

#### SD-Karte 

SD-Karte initialisieren.

#### Kamera 

* ArduCAM initialisieren.
* Überprüfe, ob es Zeit ist, ein Foto zu machen (z.B. alle 30 Minuten). Wenn ja, rufe eine Funktion auf, die ein Bild aufnimmt und mit einem Zeitstempel als Dateinamen auf der SD-Karte speichert.

#### Luft

AM2302-Sensor initialisieren.


#### Webserver

Webserver starten (server.begin()).
Eingehende Client-Anfragen handeln (server.handleClient()).

## 9. Webinterface

### Technologie-Stack

* HTML, CSS, Vanilla JS, 
* Single-Page

**Frage:** Eventuell:
* MicroPython oder Arduino C++
* ESPAsyncWebServer (für HTML/CSS/JS)
* SPIFFS oder LittleFS für lokale Webseiten-Dateien

### Routen

Frage: Besser ist es, die Routen in der REST-Konvention zu definieren, oder?

* /: Startseite, die die HTML-Seite mit den Ist-Werten anzeigt.
* /settings: Seite mit dem Formular für die Soll-Werte.
* /setsettings: Die Route, an die das Formular die Daten sendet.
* /gallery: Seite, die die Bilder auflistet.
* /data.json: Eine Route, die nur die Sensordaten im JSON-Format liefert (ideal für Live-Updates auf der Webseite).
* /capture: Eine Route, die ein Foto auslöst und speichert.

### Funktionsumfang

* Seite 1: Dashboard (index.html):
	- Zeigt die aktuellen Werte (Temperatur, Feuchtigkeit etc.).
	- Verwende JavaScript, um periodisch die /data.json Route abzufragen und die Werte auf der Seite zu aktualisieren, ohne sie neu laden zu müssen (AJAX).

* Seite 2: Einstellungen:
	- Ein einfaches HTML-Formular <form action="/setsettings" method="post"> mit Eingabefeldern für Soll-Temperatur, Soll-Feuchtigkeit, Licht-An/Aus-Zeiten etc.

* Seite 3: Galerie:
	- Der ESP32 liest beim Aufruf dieser Seite das Verzeichnis der SD-Karte aus.
	- Er generiert dynamisch eine HTML-Seite, die für jedes gefundene Bild ein <img> Tag enthält (z.B. <img src="/bild_20230415_1430.jpg" width="300">).
	- Für das Zeitraffer-Video kannst du ein JavaScript verwenden, das alle Bild-URLs in ein Array lädt und sie nacheinander in einem Image-Element anzeigt, um einen Videoeffekt zu erzeugen.

### Verzeichnisstruktur

```
ESP32
├── index.html       # Webinterface
├── style.css        # Design
├── script.js        # Interaktive Steuerung
├── main.ino         # Sensorlogik + Webserver
```

Die Dateien können mit dem ESP32 Sketch Data Upload Tool auf den ESP32 hochgeladen werden (SPIFFS oder LittleFS).

### Programmierung des Webinterfaces (Entwurf)

**index.html**

```html
// 1. Bibliotheken einbinden
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DHT.h>
// ... weitere Bibliotheken für Kamera, SD, Display ...

// 2. WLAN-Daten & PIN-Definitionen
const char* ssid = "DEIN_WLAN_NAME";
const char* password = "DEIN_WLAN_PASSWORT";
#define DHTPIN 27
#define SOILPIN 34
// ... weitere Pins für Relais ...

// 3. Variablen für Soll- und Ist-Werte
float temp_ist = 0, hum_ist = 0, soil_ist = 0;
float temp_soll = 25.0, hum_soll = 70.0, soil_soll = 50.0;
bool light_on = false;

// 4. Objekte initialisieren
DHT dht(DHTPIN, DHT22);
AsyncWebServer server(80);

// ===============================
// SETUP - Läuft einmal beim Start
// ===============================
void setup() {
	Serial.begin(115200);
	// WLAN verbinden
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) { delay(1000); Serial.println("Verbinde..."); }
	Serial.println(WiFi.localIP());
	
	// Sensoren, Display, SD-Karte, Kamera initialisieren
	dht.begin();
	// ...
	
	// Webserver-Routen definieren
	// Startseite (/) -> Zeigt Ist-Werte an
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		String html = "<h1>Gewächshaus Live</h1>";
		html += "<p>Temperatur: " + String(temp_ist) + "°C</p>";
		// ... weitere Werte ...
		request->send(200, "text/html", html);
	});
	
	// Einstellungsseite (/settings) -> Formular zum Setzen der Soll-Werte
	server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request){
		// HTML-Code für ein Formular mit Eingabefeldern senden
	});
	
	// Route zum Verarbeiten der Einstellungs-Daten
	server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
		if (request->hasParam("temp")) {
			temp_soll = request->getParam("temp")->value().toFloat();
		}
		// ... weitere Parameter verarbeiten ...
		request->send(200, "text/plain", "Werte gespeichert!");
	});
	
	// Bildergalerie (/gallery)
	// ... hier Code einfügen, der das SD-Verzeichnis liest und Links zu den Bildern erstellt ...
	
	// Webserver starten
	server.begin(); 
}

// =======================
// LOOP - Läuft unendlich
// =======================
void loop() {
	// 1. Sensoren auslesen
	temp_ist = dht.readTemperature();
	hum_ist = dht.readHumidity();
	soil_ist = analogRead(SOILPIN); // Wert muss noch kalibriert werden!
	
	// 2. Regelungslogik (Beispiele)
	
	// 2.1 Lichtsteuerung (z.B. zeitbasiert)
	// if (Uhrzeit >= 8 && Uhrzeit <= 20) { digitalWrite(LIGHT_RELAY_PIN, HIGH); } else { ... }
	
	// 2.2 Bewässerung
	if (soil_ist < soil_soll) {
		// Pumpe für 5 Sekunden einschalten
		digitalWrite(PUMP_RELAY_PIN, HIGH);
		delay(5000);
		digitalWrite(PUMP_RELAY_PIN, LOW);
	}
	
	// 2.3 Temperaturregelung
	if (temp_ist > temp_soll) {
		digitalWrite(FAN_RELAY_PIN, HIGH); // Lüfter an
	} 
	else {
		digitalWrite(FAN_RELAY_PIN, LOW); // Lüfter aus
	}
	
	// 3. Display aktualisieren
	// display.clearDisplay(); ... display.print(...); ... display.display();
	
	// 4. Foto machen (einmal jeden Tag um 12 Uhr)
	// takeAndSavePhoto();
	
	// 5 Sekunden warten vor dem nächsten Durchlauf
	delay(5000);
}
```

**Beispiel: Button zur Pumpensteuerung**

```html
<button onclick="fetch('/pump/on')">Pumpe AN</button>
<button onclick="fetch('/pump/off')">Pumpe AUS</button>
```

```cpp
server.on("/pump/on", HTTP_GET, [](AsyncWebServerRequest *request){
  digitalWrite(PUMP_PIN, HIGH);
  request->send(200, "text/plain", "Pumpe eingeschaltet");
});
```

## 10. Inbetriebnahme und Kalibrierung

### Erster Test

Öffne den Seriellen Monitor in der Arduino IDE. 
Hier solltest du die IP-Adresse deines Gewächshauses sehen, sobald es mit dem WLAN verbunden ist.

### Webinterface

Gib diese IP-Adresse in deinem Browser ein. 
Die Startseite mit den Live-Werten sollte erscheinen.

### Kalibrierung

* Bodenfeuchte: Der Bodenfeuchtesensor gibt nur einen rohen Analogwert zurück. 
Miss den Wert einmal in komplett trockener Erde und einmal in nasser Erde. 
Mit diesen beiden Werten kannst du den Messwert im Code auf eine Skala von 0-100% umrechnen (mittels der map()-Funktion).

### Regelparameter einstellen
	
Beginne mit moderaten Werten. Für tropische Pflanzen könnten das sein:

* Raumtemperatur: 24-28°C
* Bodentemperatur: 22°C
* Luftfeuchtigkeit: 60-80%
* Lichtzyklus: 06:00–20:00 Uhr (14 Stunden an, 10 Stunden aus)
* Bodenfeuchte: Halte sie konstan feucht, aber nicht nass.
* Pumpe: Aktiviert bei Bodenfeuchte < 40%

## Anhang 

s. KiCad

TODO

### Programmierumgebung 

Für die Programmierung ist die Arduino IDE erforderlich.

Treiber, wenn kein Port zur Verfügung steht:

https://www.silabs.com/interface/usb-bridges/classic/device.cp2102?tab=specs

[CP210x Windows Drivers with Serial Enumerator v6.7.6, 64-bit](https://www.silabs.com/documents/public/software/CP210x_Windows_Drivers_with_Serial_Enumeration.zip)
 
Arduino IDE einrichten:

1. ESP32 Board-Unterstützung installieren 
	* Gehe zu Datei > Voreinstellungen 
	* Füge bei "Zusätzliche Boardverwalter-URLs" diese URL ein: 
	https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
	
2. ESP32 Board-Unterstützung installieren 
	* Öffne Werkzeuge > Board > Boardverwalter
	* Suche nach "esp32" und installiere das Paket von Espressif Systems.
	
3. Board auswählen
	* Gehe zu Werkzeuge > Board 
	* Wähle unter das Board "AI Thinker ESP32-CAM" aus.
	
4. Bibliotheken installieren
	* Gehe zu Sketch > Bibliothek einbinden > Bibliotheken verwalten 
	* Installiere folgende Bibliotheken:
		* Adafruit DHT sensor library bzw. Adafruit BME280 Library
		* Adafruit GFX Library
		* Adafruit SSD1306
		* Eventuell auch diese; bin mir nicht sicher ob es notwendig ist:
			* WebServer (ist Teil des ESP32-Kerns)
			* WiFi (ist Teil des ESP32-Kerns)
			* FS.h (für Dateisystem auf der SD-Karte) 
			* Adafruit Unified Sensor (Frage: wofür ist das?)
			* AsyncTCP und ESPAsyncWebServer (diese müssen eventuell manuell als .zip installiert werden, suche nach Anleitungen online)

### Sketch hochladen

1. Verbinde den FTDI-Programmer mit dem ESP32-CAM.
2. Wichtig: Verbinde den Pin GPIO 0 des ESP32 mit GND, um ihn in den Flash-Modus zu versetzen.
3. Wähle in der Arduino IDE den richtigen COM-Port aus.
4. Klicke auf "Hochladen".
5. Nach dem Hochladen: Entferne die Verbindung zwischen GPIO 0 und GND und drücke den Reset-Knopf am ESP32, um das Programm zu starten.

### ASCII Schaltzeichen 

<pre>
Datenrichtung (Input, Output, Bidirektional)
◄── 
──► 
◄─►

Buchse
+ O──
- O──
            
IC, Sensor, Aktor
  ┌────────────────┐
──┤ In+       Out+ ├──
  │       U1       │
──┤ In-       Out- ├──
  └────────────────┘

Transistor   │ │ │
  ┌─────┐   ┌┴─┴─┴┐
  │ Q1  │   │C B E│
  │     │   │     │
  │C B E│   │ Q1  │   
  └┬─┬─┬┘   └─────┘
   │ │ │

Kreuzung:
─┼─

Widerstand
───[ R1 ]───

Kondensator
───┤├─── 
   C1

Elko
──⁺▌|───
+  C2  -  (das längere Beinchen ist der Pluspol)

Diode
───▶├───
+  D1  - (Ring)
   
LED
    ↗↗
───▶├───
+  LD1 -  (das längere Beinchen ist der Pluspol)
   
Fotodiode
   ↙↙
───▶├────
+  PD1  -

Lampe
───(╳)───
   HL1

</pre>

### Verwendete Präfixe im Schaltplan

* U: Mikrocontroller, ICs
* K: Relais (Kontaktor)
* S: Sensor
* A: Aktor
* Z: Display
* M: SD-Karte
* R: Widerstand (Resistor)
* C: Kondensator/Elko (Capacitor)
* P: Stromversorgung (Power Supply)
* Q: Transistor
* D: Diode
* H: Lampe
* J: Anschluss (Stiftleiste, Buchsenleiste, DC-Holhbuchse,...) 


### ESP32-GPIOs

Es wird die Standard-Pin-Belegung der Arduino IDE verwendet.

Folgende GPIOs stehen beim ESP32-DevKitC V2 zur Verfügung, wenn das WLAN-Modul aktiviert ist:

<pre>  
                            ┌────────────────┐
                +3.3V Out ──┤ 3V3 ESP32  GND ├── GND
                  (RESET) ──┤ EN  DevKit G23 ├── GPIO23 I/O SPI_MOSI
         ADC1-0 In GPIO36 ──┤ SP  C V2   G22 ├── GPIO22 I/O I2C_SCL
         ADC1-3 In GPIO39 ──┤ SN         TXD ├── GPIO1 (UART0_TX)
         ADC1-6 In GPIO34 ──┤ G34        RXD ├── GPIO3 (UART0_RX)
         ADC1-7 In GPIO35 ──┤ G35        G21 ├── GPIO21 I/O I2C_SDA
Touch-9 ADC1-4 I/O GPIO32 ──┤ G32        GND ├── GND
Touch-8 ADC1-5 I/O GPIO33 ──┤ G33        G19 ├── GPIO19 I/O SPI_MISO
          DAC1 I/O GPIO25 ──┤ G25        G18 ├── GPIO18 I/O SPI_CLK
          DAC2 I/O GPIO26 ──┤ G26         G5 ├── GPIO5 (Boot)
               I/O GPIO27 ──┤ G27        G17 ├── GPIO17 I/O UART2_TX
        (JTAG_TMS) GPIO14 ──┤ G14        G16 ├── GPIO16 I/O UART2_RX
  (Boot, JTAG_TDI) GPIO12 ──┤ G12         G4 ├── GPIO4 I/O
                      GND ──┤ GND         G0 ├── GPIO0 (Boot) 
        (JTAG_TCK) GPIO13 ──┤ G13         G2 ├── GPIO2 (Boot) 
            (Flash) GPIO9 ──┤ SD2        G15 ├── GPIO15 (Boot, JTAG_TDO) 
           (Flash) GPIO10 ──┤ SD3        SD1 ├── GPIO8 (Flash)
           (Flash) GPIO11 ──┤ CMD        SD0 ├── GPIO7 (Flash)
                   +5V In ──┤ 5V  [USB]  CLK ├── GPIO6 (Flash)
                            └────────────────┘
</pre>

**Legende:**

* **I/O:** Digitale Ein- und Ausgänge. Alle Ausgänge können auch PWM-Signale erzeugen.
* **In:**  Nur digitale Eingänge (GPIO 34 bis 36, 39). Nur für passive Sensoren geeignet.
* **ADC1 Analog-Digital-Wandler 1:**  Analoge Eingänge (12 Bit). ADC2 (GPIO 0, 2, 4, 12 bis 15 und 25 bis 27) ist mit dem WLAN-Modul nicht zuverlässig, bleibt daher im Diagramm unerwähnt.
* **DAC1, DAC2 (Digital-Analog-Wandler 1 und 2):** Analoge Ausgänge (8 Bit).
* **I2C:** I²C-Schnittstelle
* **SPI:** Virtual SPI-Schnittstelle. Hardware SPI wird selten gebraucht, bleibt daher im Diagramm unerwähnt.
* **Touch:** Interne kapazitive Touch-Sensoren. Die übrigen Touch-Sensoren werden aber durch das WLAN-Modul blockiert (GPIO 0, 2, 4, 12 bis 15 und 27).
* **(UART0), UART2:** Serielle Kommunikation. UART0 besser nicht verwenden. Die Pins sind intern mit dem USB-Wandler-Chip verdrahtet. UART2 ist unbenutzt. UART1 hat standardmäßig keine Pins zugewiesen (kann frei auf irgendwelche GPIOs gemappt werden).
* **(RESET):** Immer frei lassen!
* **(Flash):** GPIO 6 bis 11 sind intern mit dem Flash-Speicher verbunden. Diese Pins niemals verwenden!
* **(Boot):** GPIO 0, 2, 5, 12, 15 sind "Strapping-Pins". Besser nicht verwenden, denn der Bootvorgang wird hiermit beeinflusst. Könnten aber zur Not als digitale Ein- und Ausgänge verwenden werden (Bedingungen siehe nachfolgende Tabelle). 
* **(JTAG):** GPIO 12, 13, 14, 15 können als digitale Ein- und Ausgänge verwendet werden, sofern kein Debugger (ESP-Prog) angeschlossen wird.

**Anmerkungen:**

* Der 5V-Pin versorgt den Onboard-Regler, der daraus 3.3V erzeugt. Toleranzbereich: 4.75 - 5.25V
* Max. Stromabgabe am 3.3V-Pin: 500mA
* Die GPIO Pins (General Purpose Input/Output Pins) arbeiten mit min. 2.3 bis max. 3.6 V, empfohlen werden 3.3V. 
	=> Das heißt: 
  * Sensoren mit 3.3V Betriebsspannung können direkt angeschlossen werden. 
  * Für Sensoren mit einem Ausgangspegel über 3.3V muss der Pegel z.B. durch einen Spannungsteiler (bestehend aus zwei Widerständen) auf 3.3V gedrückt werden!
* Laut Datenblatt (5.3 DC Characteristics) ist der Schwellwert für LOW/HIGH dieser:
  * Alles unter 0.825V wird sicher als LOW erkannt.
  * Alles über 2.475V wird sicher als HIGH erkannt.
* Der absolute Maximalstrom, der pro GPIO gezogen werden darf, beträgt 40 mA.
* Für die SPI-Schnittstelle ist GPIO 5 für CS (Chip Select; synonym für SS, Slave Select) vorgegeben. 
  Das ist ungünstig, denn beim Booten muss der Pin HIGH sein. 
  Daher besser, einen anderen digitalen Ausgang für CS zu wählen (und zwar pro Gerät einen).

**Strapping-Pins:**

| Pin     | Funktion             | Bedingung                                  |
|---------|----------------------|--------------------------------------------|
| GPIO 0  | Boot-Modus           | Der Pin darf beim Booten nicht LOW sein.   | 
| GPIO 2  | Boot-Modus & Logging | Der Pin darf beim Booten nicht LOW sein.   | 
| GPIO 5  | Flash-Spannung       | Der Pin darf beim Booten nicht HIGH sein.  | 
| GPIO 12 | Flash-Spannung       | Der Pin darf beim Booten nicht HIGH sein.  | 
| GPIO 15 | Boot-Logging         | Der Pin darf beim Booten nicht LOW sein.   | 

**Schottky-Diode:**

Wenn das USB-Kabel und eine externe Spannungsversorgung an 5V-Pin angeschlossen sind, 
muss die externe Versorgung stabil sein. Wenn mehr als 5.2V am 5V-Pin anliegen, 
kann ein Rückstrom in das USB-Kabel fließen. Andersherum kann ein Rückstrom in das Netzteil fließen.  

Eine Schottky-Diode schützt das externe Netzteil vor einem Rückstrom:

<pre>
                 1N5817
Netzteil +5V ──────▶├────── ESP32_5V
                +  D1  - (Ring)
</pre>

1N5817:
* 20V Sperrspannung
* 1 A Dauerstrom
* ~0.3-0.5V Spannungsabfall

Allerdings liegt dann ca. 0.4V weniger am ESP32 an.

**Am besten einfach die externe Spannungsversorgung abklemmen, bevor das USB-Kabel eingesteckt wird!** 

### Spannungsteiler

Falls ein Sensor eine Betriebsspannung über 3.3 V hat, muss ein Spannungsteiler sicherstellen, 
dass der HIGH-Pegel nicht 3.3V überschreitet.

Beispiel - 5V-Sensor mit 5V als HIGH-Pegel:

<pre>
5V ──[ R1 ]─┬─[ R2 ]── GND
            │
          3.3V
</pre>

Teilverhältnis R2 / (R1 + R2) = 3.3 / 5 = 0.66

Faustregel: Der Gesamtwiderstand sollte im Bereich 10kΩ bis 100kΩ liegen.

Ich wähle 10 kΩ und 20 kΩ.
Die max. Ausgangsspannung ist somit 5V * (20kΩ / (20kΩ + 10kΩ) ≈ 3.33 V.

**Pinbelegung:**
<pre>
┌────────────────┐
│ V5-Sensor  GND ├──(schwarz)─O── GND 
│            VCC ├──────(rot)─O── +5V
│            OUT ├─►───(gelb)─O──[ R1 ]─┬─[ R2 ]── GND
└────────────────┘                      └────────► GPIO
                                    R1=10k ±1%  
                                    R2=20k ±1%
</pre>  

### ESP-Prog

ESP-Prog ist ein Debugging-Tool von Espressif, basierend auf dem FT2232HL-Chip.

![ESP-Prog](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/_images/modules.png)

[Benutzerhandbuch](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/esp-prog/user_guide.html), 
[H/W-Referenz](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/esp-prog/reference/reference-doc.html)

[Erste Schritte mit PlatformIO](https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html)

Der ESP-Prog stellt über ein einziges USB-Kabel zwei Schnittstellen bereit (per FTDI FT2232HL Chip):

* Interface 0 (JTAG-Schnittstelle): für Upload und Debugging via Standard-Debugging-Tool OpenOCD
* Interface 1 (UART-Schnittstelle): für Serielle Kommunikation (Serial Monitor/Plotter) via esptool.py

Anmerkung zur Upload-Funktion:

Das Hochladen des Codes kann auch so wie in der Arduino-IDE mittels esptool.py erfolgen (also über die UART-Schnittstelle). 
Dazu muss in der platformio.ini "upload_protocol = esptool" eingetragen werden (oder ausklammern, da das der Standardwert ist).
Aber diese Möglichkeit ziehe ich nicht weiter in Betracht.

**Kommunikationswege:**

* Upload und Debug: PC -> USB -> ESP-Prog -> Interface 0 (JTAG) -> ESP32
* Serieller Monitor: ESP32 -> Interface 1 (UART) -> ESP-Prog -> USB -> PC

**Erforderliche Treiber unter Windows:**

* Für **Interface 0** muss der FTDI-Treiber durch einen WinUSB-Treiber ersetzt werden, 
weil das OpenOCD nicht direkt mit dem FTDI-Treiber kommunizieren kann.

  Das geschieht so:

  1) [VCP-Treiber](https://ftdichip.com/drivers/vcp-drivers/) herunterladen und installieren
  ![VCP-Treiber](https://blog.berrybase.de/wp-content/uploads/2024/10/JTag-Debugger_VCP-Driver-1024x736.png.webp)
  2) Mit dem [Zadig-Tool](https://zadig.akeo.ie/) für "Dual RS232-HS (Interface 0)" den Treiber "FIDIBUS (v2.12.36.20)" mit "WinUSB (v6.1.7600.16385)" ersetzen. 
  Nachtrag: Der Upload läuft damit nicht zuverlässig. Hab nun "libusbK (v3.1.0.0)" installiert.
  ![Zadig](https://blog.berrybase.de/wp-content/uploads/2024/10/JTag-Debugger_Update-driver.png.webp)
  3 ) USB-Kabel aus und wieder einstecken.

* Für **Interface 1** wird der benötigte FTDI-Treiber automatisch installiert, wenn der ESP-Prog zum ersten Mal angeschlossen wird.

Nach erfolgreicher Installation steht im Gerätemanager unter "USB-Geräte" der Eintrag "Dual RS232-HS" (für Interface 0, JTAG) und 
unter "Anschlüsse (COM & LPT)" der Eintrag "USB Serail Port (COM6)" (für Interface 1, UART).

Quelle: [BerryBase](https://blog.berrybase.de/esp32-debuggen-mit-espressif-esp-prog-platformio/)

**Pinbelegung der PROG-Buchse:**

![PROG-Schnittstelle](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/_images/program_pin.png)

| Pin am ESP-Prog |               | Farbe   | Pin am ESP32              |   
|-----------------|---------------|---------|---------------------------|
| 1 EN            | Reset         | schwarz | EN                        |                | 
| 2 VDD           | VCC           | rot     | 3.3V oder nicht verbunden | 
| 3 TXD           | UART Transmit | weiß    | GPIO1 (TXD)               |  
| 4 GND           | GND           | blau    | GND                       |
| 5 RXD           | UART Receive  | grau    | GPIO3 (RXD)               |   
| 6 IO0           | Strapping Pin | lila    | GPIO0                     |    

**Pinbelegung der JTAG-Buchse (für Debugging):**

![JTAG-Schnittstelle](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/_images/JTAG_pin.png)

| Pin am ESP-Prog |                 | Farbe  | Pin am ESP32              |   
|-----------------|-----------------|--------|---------------------------|
| 1 VDD           | VCC             | rot    | 3.3V oder nicht verbunden | 
| 2 TMS           | Test Mode State | braun  | GPIO14                    | 
| 4 TCK           | Test Clock      | orange | GPIO13                    |  
| 6 TDO           | Test Data Out   | gelb   | GPIO15                    |   
| 8 TDI           | Test Data In    | grün   | GPIO12                    |    
| 3,5,7,9 GND     | GND             | blau   | GND                       | 

**Der PWR-SEL-Jumper muss auf 3.3V gesetzt sein.**

Auf 5V darf der Jumper nicht gesteckt werden, da sonst an einigen Pins mehr als 3.3 V liegen und der ESP32 darauf verschnupft reagieren könnte.

**Stromversorgung:**

Der ESP-Prog wird über das USB-Kabel versorgt. Er kann den ESP32 mitversorgen, indem der 3.3V-Pin verbunden wird. 
Wenn aber eine andere Stromquelle für den ESP32 angeschlossen ist, darf der 3.3V-Pin nicht verbunden werden!

**Anschlusskabel:**

* Flachbandkabel (Ribbon Cable, meist 1.27mm oder 2.54mm Pitch)
* IDC-Stecker (2×3 und 2×5, Pfostenverbinder mit Schneidklemmen, auch Wannenstecker genannt)

Die Klemmen kann man mittels Schraubstock zusammenpressen.

**Konfiguration:**

```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

; Debug- & Upload- Konfiguration
debug_tool = esp-prog
upload_protocol = esp-prog ; Upload über Inerface 0 (JTAG)
; upload_protocol = esptool ; Upload über Inerface 1 (UART)

; Serielle Monitor Konfiguration
monitor_port = COM6 ; siehe Gerätemanager unter "Anschlüsse (COM & LPT)" oder PlatformIO unter PIO Home -> Devices
monitor_speed = 115200

; Level 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug, 5=Verbose
; siehe https://docs.platformio.org/en/latest/platforms/espressif32.html
build_flags = -DDEBUG_BAUDRATE=115200 ;-DCORE_DEBUG_LEVEL=1

; Enthält Einstellungen für den Build-Prozess während des Debuggens, wie z.B. O0 und ggdb, 
; um die Sichtbarkeit aller Variablen zu gewährleisten.
debug_build_flags = -O0 -g -ggdb

; Ermöglicht das Setzen von Breakpoints in der Setup-Funktion, was besonders nützlich ist, 
; um in der Initialisierungsphase zu debuggen.
debug_init_break = tbreak setup
```

Quellen:
[PlatformIO](https://docs.platformio.org/en/stable/plus/debug-tools/esp-prog.html),
[BerryBase](https://blog.berrybase.de/esp32-debuggen-mit-espressif-esp-prog-platformio/),
[Harald Kreuzer](https://www.haraldkreuzer.net/aktuelles/esp32-jtag-debugging-mit-platformio-und-nur-einer-usb-verbindung)

**Troubleshooting:**

OpenOCD testen:

```bash
& "C:\Users\frank\.platformio\packages\tool-openocd-esp32\bin\openocd.exe" -d2 -s "C:\Users\frank\.platformio\packages\tool-openocd-esp32\share\openocd\scripts" -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg -c "adapter speed 1000"
```

Reset‑Check mit OpenOCD:

```bash
& "C:\Users\frank\.platformio\packages\tool-openocd-esp32\bin\openocd.exe" `
  -d2 -s "C:\Users\frank\.platformio\packages\tool-openocd-esp32\share\openocd\scripts" `
  -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg `
  -c "adapter speed 1000" -c "init" -c "reset halt" -c "exit"
```

Flash-Kommando:

```bash
& "C:\Users\frank\.platformio\packages\tool-openocd-esp32\bin\openocd.exe" `
  -d2 -s "C:\Users\frank\.platformio\packages\tool-openocd-esp32\share\openocd\scripts" `
  -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp-wroom-32.cfg `
  -c "adapter speed 1000" `
  -c "init" `
  -c "reset halt" `
  -c "program_esp C:/Users/frank/Source/PlatformIO/biodom-mini/.pio/build/debug/firmware.bin 0x10000 verify" `
  -c "reset run" `
  -c "shutdown"
```  

Upload via PlatformIO-Befehl:  

```bash
pio run -e debug -t upload
```

ESP32 Chip-ID auslesen:

```bash
python -m esptool chip-id
```

Versionen einer Bibliothek auflisten:


```bash
pio lib show OneWire
```

### Kabelgedöns

**System Dupont:**

* Stiftleisten und Buchsenleisten zum Einlöten auf die Platine 
* 
* Auch unter "Pinheader" zu finden.

  ![Stift- und Buchenleisten](https://m.media-amazon.com/images/I/71f849Jd+9L._SX522_.jpg)

  [Amazon](https://www.amazon.de/dp/B0BZH6YZF8)
  10,99 €

* Jumper-Kabel

  ![Jumper Kabel](https://m.media-amazon.com/images/I/81k65hd9GOL._SX466_.jpg)

  Der Leiterquerschnitt ist üblicherweise 0,25mm² (AWG 24 oder AWG 26??). 

  Männlich zu männlich:
  [Amazon](https://www.amazon.de/dp/B0FJ8487X1)
  7,99 €

  Männlich zu weiblich:
  [Amazon](https://www.amazon.de/dp/B0FJ85CY1L)
  7,99 €

  Weiblich zu weiblich:
  [Amazon](https://www.amazon.de/dp/B0F98KBV61)
  7,99 €

**Andere Systeme:**

* IDC (Wannenstecker, Pfostenbuchsen)
* JST-XH (Zusatz "-XH" ist hier wichtig!)
* Molex

### Sonstiges Zubehör

**Kabelschlauch, selbstschließend**

![Kabelschlauch](https://m.media-amazon.com/images/I/71t6eXtVD6L._AC_SY450_.jpg)

6 mm Durchmesser, 3m: 
[Amazon](https://www.amazon.de/dp/B07FW5H57B)
7,99 €

13 mm Durchmesser, 3m: 
[Amazon](https://www.amazon.de/dp/B07FW3GTXB)
7,99 €

**Abstandhalter Set**

![Abstandhalter](https://m.media-amazon.com/images/I/813e8a4aQJL._SX466_.jpg)

[Amazon](https://www.amazon.de/dp/B0DCS5C7SN)
8,99 €

**Schraubklemmen Set**
![Schraubklemmen](https://m.media-amazon.com/images/I/61ocvyBAO2L._SX679_.jpg)
[Amazon](https://www.amazon.de/dp/B082ZNRQMJ)
8,99€

### KiCad Libs

Libs herunterladen und installieren:

Offizielle KiCad-Bibliothek von Espressif (Hersteller von ESP32-Lib) herunterladen. 
Auf der Github.Seite steht, wie es installiert wird.
https://github.com/espressif/kicad-libraries?tab=readme-ov-file

KiCad-Bibliothek von AZDelivery herunterladen und entpacken.
In der Readme steht, wie es installiert wird.
https://github.com/nkappler/KiCAD-AZDelivery

SSD1306 128x64px OLED Display
https://github.com/pforrmi/KiCad-SSD1306-128x64

Buchsenleisten
Standard-Footprints: Connector_PinSocket_2.54mm 
z.B. PinSocket_1x04_P2.54mm_Vertical 
für 1 Reihe 4 Pins


# Projekt "Biodom Mini" – Ein Hightech-Gewächshaus

* **Version:** 1.0.2
* **Datum:** 11.11.2025
* **Autor:** Frank Rohlfing

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
  * [7. Schaltplan](#7-schaltplan)
  * [8. Programmierung](#8-programmierung)

  * [Anhang](#anhang)

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

5 m Schlauch, 10 Tropf-Sprinkler, 2 Ventile, Verbinder (reicht locker für 2 Häuschen)

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
* Rastermaß: 2,54 mm
* 1,6 mm dick

### Klinkenstecker und -Buchsen für die Sensoren (Kamera ausgenommen)

3.5 mm Stereo-Klinkenstecker und -Buchse 

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

DC Power Jack Buchse und Stecker, 2.1 mm, 2 Polig 

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

### Sonstiges Zubehör

**Stiftleisten und Buchsenleisten zum Einlöten auf die Platine** 

Auch unter "Pinheader" zu finden.

  ![Stift- und Buchenleisten](https://m.media-amazon.com/images/I/71f849Jd+9L._SX522_.jpg)

  [Amazon](https://www.amazon.de/dp/B0BZH6YZF8)
  10,99 €

**Kabelschlauch, selbstschließend**

![Kabelschlauch](https://m.media-amazon.com/images/I/71t6eXtVD6L._AC_SY450_.jpg)

6 mm Durchmesser, 3 m: 
[Amazon](https://www.amazon.de/dp/B07FW5H57B)
7,99 €

13 mm Durchmesser, 3 m: 
[Amazon](https://www.amazon.de/dp/B07FW3GTXB)
7,99 €

**Abstandhalter Set**

![Abstandhalter](https://m.media-amazon.com/images/I/813e8a4aQJL._SX466_.jpg)

[Amazon](https://www.amazon.de/dp/B0DCS5C7SN)
8,99 €

**Schraubklemmen Set**
![Schraubklemmen](https://m.media-amazon.com/images/I/61ocvyBAO2L._SX679_.jpg)
[Amazon](https://www.amazon.de/dp/B082ZNRQMJ)
8,99 €

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
  Reserve analog./dgl. In ──► GPIO36 ──┤ SP  C V2   G22 ├── GPIO22 I2C_SCL  ──► Kamera + Display + Lichtstärke
       (frei analog./dgl. In) GPIO39 ──┤ SN         TXD ├── GPIO1  (UART0_TX)
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
  Luft (One-Wire-ähnlich) ◄─► GPIO13 ──┤ G13         G2 ├── (frei, Boot)
                                     ──┤ SD2        G15 ├── (frei, Boot)
                                     ──┤ SD3        SD1 ├──
                                     ──┤ CMD        SD0 ├──
                              +5V in ──┤ 5V  [USB]  CLK ├──
                                       └────────────────┘
</pre>

### Relaismodul

8-Kanal Relaismodul mit Optokoppler, 5V

[Amazon](https://www.amazon.de/dp/B07CT7SLYQ)
9,59 €

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

**Anmerkung:**

Der Jumper zwischen VCC und JD-VCC muss gesteckt sein, da die Relais-Spulen auch von Steuerstromkreis versorgt werden sollen.

Um Induktionsspitzen beim Schalten abzufangen, wird ein Stützkondensator beim Spannungswandler eingesetzt (s. Kapitel6).

## 3. Sensoren

| Präfix | Name                                        | Spezifikation               | GPIO   | Einkauf                                                                             |
|--------|---------------------------------------------|-----------------------------|--------|-------------------------------------------------------------------------------------|
| S1     | Raumtemperatur- und Luftfeuchtigkeitssensor | AM2302 Module               | GPIO13 | [Amazon](https://www.amazon.de/dp/B078SVZB1X) 8,49 €                                |
| S2     | Bodentemperatursensor                       | DS18B20                     | GPIO4  | [Amazon](https://www.amazon.de/dp/B01MZG48OE) 8,29 € (2 Stück); Einzelpreis: 4,15 € |
| S3     | Bodenfeuchtigkeitssensor                    | Capacitive Soil Sensor v1.2 | GPIO34 | [Amazon](https://www.amazon.de/dp/B07HJ6N1S4) 6,46 €                                |
| S4     | Füllstandsensor                             | XKC-Y25-NPN                 | GPIO35 | [Amazon](https://www.amazon.de/dp/B0C1ZVZF2L) 10,19 €                               |
| S5     | Lichtsensor                                 | GY-302 BH1750               | (I2C)  | [Amazon](https://www.amazon.de/dp/B07TKWNGZ4) 5,49 €                                |

### S1: AM2302
<pre>
┌────────────────┐
│ AM2302       - ├──(schwarz)─O─── GND 
│ Module     Out ├─►───(gelb)─O──► GPIO13
│              + ├──────(rot)─O─── +3.3V
└────────────────┘             
</pre>

### S2: DS18B20
<pre>
┌────────────────┐                   R1=4.7k
│ DS18B20    GND ├──(schwarz)─O────────────────── GND 
│           DATA ├─►───(gelb)─O─────┬───────────► GPIO4
│            VCC ├──────(rot)─O──┐  └─[ R1 ]─┐
└────────────────┘               └───────────┴─── +3.3V
</pre>

Pull-Up-Widerstand R1 = 4.7 kΩ

### S3: Capacitive Soil Sensor v1.2
<pre>
┌─────────────────┐
│ Capacitive  GND ├──(schwarz)─O─── GND 
│ Hygrometer  VCC ├──────(rot)─O─── +5V
│ v1.2       AOUT ├─►───(gelb)─O─►─ GPIO34
└─────────────────┘
</pre>

### S4: XKC-Y25-NPN

| Pin | Bezeichnung | Farbe am Sensor | Farbe am Klinkenstecker | Beschreibung                             |
|-----|-------------|-----------------|-------------------------|------------------------------------------|
| 1   | VCC         | braun           | rot                     | Versorgung (5–12V)                       |  
| 2   | OUT         | gelb            | grün                    | Digitaler Ausgang                        | 
| 3   | GND         | blau            | weiß                    | GND                                      |
| 4   | MODE        | schwarz         | -                       | Modus-Umschaltung (wird nicht benötigt)  | 

Eventuell sind Schwarz und Gelb vertauscht (OUT = Schwarz, MODE = Gelb)

<pre>
┌────────────────┐                     R2=10k 
│ XKC-Y25    VCC ├────(braun)─O───────────────── +5V   
│ -NPN       OUT ├─►───(gelb)─O─────┬──[ R2 ]─── +3.3V 
│            GND ├─────(blau)─O───┐ └─────────►─ GPIO35    
│           MODE ├──(schwarz)─O   └───────────── GND         
└────────────────┘    
</pre>

Pull-Up-Widerstand R2 = 10 kΩ

#### S5: BH1750
<pre>
┌────────────────┐
│ GY-302     VDD ├──────(rot)─O── +3.3V
│ BH1750     GND ├──(schwarz)─O── GND
│            SCL ├────(braun)─O─◄─ GPIO22
│            SDA ├───(orange)─O◄─► GPIO21 
│           ADDR ├─────(gelb)─O──
└────────────────┘
</pre>

## 4. Aktoren

| Präfix | Name      | GPIO   |
|--------|-----------|--------|
| A1     | Lampe 1   | GPIO14 | 
| A2     | Lampe 2   | GPIO27 |
| A3     | Heizer    | GPIO26 |
| A4     | Lüfter    | GPIO25 |
| A5     | Pumpe     | GPIO33 |
| A6     | Vernebler | GPIO32 |

### A1, A2: Lampe

LED Grow Light 12V/60W, Vollspektrum, 4:1, IP65

![LED-Streifen](https://m.media-amazon.com/images/I/81TPLiD5MnL._AC_SX679_.jpg)

[Amazon](https://www.amazon.de/gp/product/B076D3GCH4)
18,99 €

* 12V, 60W
* Rot:Blau = 4:1
* Vollspektrum
* IP65 (wasserdicht)
* 5m-Streifen (1 cm breit) mit 300 LEDs (SMD 5050)
* Hersteller: Tesfish

12V/60W = 5A (lt. Hersteller) -> Damit ist das Limit für ein Relais des Relais-Moduls erreicht. Daher teile ich den 
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

Der gemessene Strom ist unter 4A, es würde 1 Relais reichen. (Sicherheitsmarge von 20 % ist ok). 
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

Wird mit 1 m Schlauch geliefert (5 mm innen).

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

| Präfix | Name           | Spezifikation                  | GPIO       | Einkauf                                                                                                                                                                    |
|--------|----------------|--------------------------------|------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Z1     | Display        | 1.3 Zoll OLED Display, SSH1106 | (I2C)      | [Amazon](https://www.amazon.de/dp/B078J78R45) 8,49 €                                                                                                                       |
| Z2     | SD-Kartenleser | MicroSD SPI Kartenleser        | CS: GPIO16 | [Amazon](https://www.amazon.de/gp/product/B0DQZFBCST) 4,29 € (3 Stück); Einzelpreis: 1,43 €                                                                                |
| Z3     | Kamera         | ArduCAM Mini 2MP Plus, OV2640  | CS: GPIO17 | [BotLand](https://botland.de/kameras-fur-arduino-und-raspberry-pi/6556-arducam-mini-ov2640-2mpx-1600x1200px-60fps-spi-kameramodul-fur-arduino--5904422358242.html) 28,86 € |
| Z4     | LED            | LED 3mm, rot                   | GPIO5      |                                                                                                                                                                            |

### Z1: Display

<pre>
┌────────────────┐
│ 1.3″       VDD ├──────(rot)─O── +3.3V
│ OLED       GND ├──(schwarz)─O── GND
│ Display    SCK ├────(braun)─O─◄─ GPIO22
│ SSH1106    SDA ├───(orange)─O◄─► GPIO21 
└────────────────┘
</pre>

### Z2: SD-Kartenleser

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

* 93 % einer SD-Karte sind durchschnittlich praktisch nutzbar.
* Die höchste Auflösung der ArduCAM-Kamera ist 1600 x 1200 Pixel. Die Bilder werden im JPEG-Format gespeichert. 
  Ich schätze 600 KB / Bild durchschnittlich.  

Es folgt:
* 4 GB: 4 GB * 0,93 * 1024 * 1024 / 600 KB = ≈ 6500 Bilder => 6500 / 24 ≈ **270 Tage**
* 32 GB: 32 GB * 0,93 * 1024 * 1024 / 600 KB = ≈ 52000 Bilder => 52000 / 24 / 365 ≈ **6 Jahre**

Theoretische Schreibgeschwindigkeit pro Bild:
* Class 4 (min. 4 MB/s): 0.6 MB / 4 MB/s = 150 ms. 
* Class 10 (min. 14 MB/s): 0.6 MB / 10 MB/s = 60 ms.

In der Realität ist der Prozess durch den ESP32 und die SPI-Kommunikation limitiert. 
Der gesamte Vorgang (Auslesen von der Kamera + Schreiben auf die Karte) wird vermutlich eher bei 1 bis 1,5 Sekunden liegen.

### Z3: Kamera

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

### Z4: LED

Leuchtdiode 3mm, rot 

Nützlich zum Debuggen

Das längere Beinchen ist der Pluspol.

**Pinbelegung mit Vorwiderstand:**

<pre>
                      ↗↗     
GPIO5 ─▶───[ R2 ]─────▶├───── GND
            330 Ω    LED1
</pre>

## 6. Stromversorgung

| Präfix | Name                       | Spezifikation                     | Einkauf                                                                                             |
|--------|----------------------------|-----------------------------------|-----------------------------------------------------------------------------------------------------|
| P1     | Spannungswandler 12V zu 5V | Step-Down-Module XL4015           | [Amazon](https://www.amazon.de/dp/B07VQCXDTC) 9,99 € (3 Stück); Einzelpreis:	3,33 €                 |
| P2     | 12V Netzteil, 120W         | 12V Steckernetzteil Adapter, 120W | [Amazon](https://www.amazon.de/gp/product/B0DBL4FV6C) 11,99 € 9,99 € (3 Stück); Einzelpreis:	3,33 € |

### P1: Spannungswandler 12V zu 5V

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

Die Ausgangsspannung muss auf **5 Volt** eingestellt werden!

### Netzteil

12V Steckernetzteil Adapter, 120W

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

## 8. Programmierung

### Entwicklungsumgebung / Technologie-Stack

* PlatformUI mit VS Code
* ArduinoIDE (nur für den erster schneller Funktionstest und Proof Of Concepts, die Sketches liegen im Verzeichnis `sketches`)
* ESP-Prog für das Debuggen (sofern nötig)
* Webinterface via Websocket (Frontend mit HTML, CSS, Vanilla JS, Single-Page)
* OTA-Upload (Over-The-Air) für das Produktivsystem

Mit der PlatformUI wird für jedes Bauteil eine lokale Bibliothek bereitgestellt und ausführlich dokumentiert.

Es wird auch jeweils eine Bibliothek für das Webinterface, für OTA und für den Zugriff auf die Sollwerte (als persistentes Dictionary o.ä.) bereitgestellt (um `main.cpp` möglichst schlank halten zu können). 

### Steuerungslogik

Die Steuerung soll dafür sorgen, dass ein gleichmäßiges Klima für die Pflanzen im Gewächshaus herrscht. 

Es wird folgende Steuerungslogik programmiert (in `main.cpp`):

*   **Lichtsteuerung (A1 und A2):**  
	* **Zur Tageszeit** werden die Lampen eingeschaltet, wenn das natürliche Tageslicht nicht ausreicht. 
	Dazu misst der Lichtsensor (S5) kontinuierlich das Tageslicht. 
	Ist das Tageslicht dunkler als der Schwellwert `LIGHT_LUX_THRESHOLD_DARK`, wird die Lampe eingeschaltet.
	Ist das Tageslicht heller als der Schwellwert `LIGHT_LUX_THRESHOLD_BRIGHT`, wird die Lampe ausgeschaltet.
    * **Außerhalb der Tageszeit** bleiben die Lampen aus.
    * Die Tageszeit ist definiert durch Einschaltzeit `LIGHT_ON_HOUR` und Ausschaltzeit `LIGHT_OFF_HOUR`.
    * Lampe 1 und Lampe 2 werden unabhängig voneinander gesteuert, d.h., für die Lampen können unterschiedliche Tageszeiten und Schwellwerte haben. 
  
* **Heizungssteuerung (A3):**  
Die Heizmatte wird aktiviert, wenn die **Bodentemperatur (S2)** unter den Zielwert `SOIL_TEMPERATUR_TARGET` fällt. Um ein ständiges An- und Ausschalten (Flattern) zu verhindern, wird eine Hysterese implementiert: Die Heizung schaltet sich erst wieder aus, wenn die Temperatur den Zielwert um 0.5 °C überschreitet.

*   **Lüftersteuerung (A4):**
Der Lüfter wird aktiviert, wenn die **Raumtemperatur (S1)** den Schwellwert `AIR_TEMPERATUR_THRESHOLD_HIGH` überschreitet. Er läuft dann für eine feste Dauer von `FAN_COOLDOWN_DURATION_MS`, um die warme und feuchte Luft effektiv abzuführen, und schaltet sich danach automatisch ab. Dies verhindert eine zu schnelle Austrocknung durch Dauerbetrieb.

*   **Bewässerungssteuerung (A5):** 
Die Wasserpumpe wird aktiviert, wenn die **Bodenfeuchte (S3)** unter den Zielwert `SOIL_MOISTURE_TARGET` fällt. Die Pumpe läuft dann für eine feste Dauer von `WATERING_DURATION_MS`, um eine definierte Menge Wasser abzugeben.  
**Ausnahme:** Die Pumpe wird niemals aktiviert, wenn der **Wasserstandsensor (S4)** meldet, dass der Wasserbehälter leer ist, um ein Trockenlaufen zu verhindern.

*   **Luftfeuchtesteuerung (A6):** Der Ultraschall-Vernebler (Dunst) wird eingeschaltet, wenn die **relative Luftfeuchtigkeit (S1)** unter den Zielwert `HUMIDITY_TARGET` sinkt. Auch hier sorgt eine Hysterese dafür, dass er sich erst wieder abschaltet, wenn die Luftfeuchtigkeit 5 % über dem Zielwert liegt. 
**Ausnahme:** Der Vernebler wird nur aktiviert, wenn der **Wasserstandsensor (S4)** ausreichend Wasser meldet.

*   **Wasserstands-Überwachung (S4):** Der Füllstandsensor wird kontinuierlich überwacht. Wenn er einen niedrigen Wasserstand (`!WATER_LEVEL_TRIGGERED`) meldet, wird auf dem Display eine bildschirmfüllende, blinkende Warnung angezeigt. Zusätzlich werden Pumpe (A5) und Vernebler (A6) deaktiviert.

*   **Kamerasteuerung (Z3):** In einem festen Intervall (z.B. alle 60 Minuten) wird ein Foto in hoher Auflösung (1600x1200) aufgenommen und auf der SD-Karte (Z2) gespeichert. Der Dateiname enthält einen Zeitstempel, um eine chronologische Sortierung zu ermöglichen (z.B. `img_20241017_143000.jpg`).

### Optimale Klimawerte

Eine Internet-Recherche über optimale Klimawerte für tropische Pflanzen ergab Folgendes:

*   **Temperatur:** 
    - Tropische Pflanzen, die oft aus Regenwäldern stammen, benötigen ein stabiles Klima mit hoher Luftfeuchtigkeit und gleichmäßig hohen Temperaturen. [1]

    - Für tropische Pflanzen sind Tagestemperaturen zwischen 20 °C und 30 °C ideal. [2]

    - Nachts sollten die Temperaturen nicht wesentlich unter 20 °C fallen. [3]

    - Die Bodentemperatur ist für die Wurzelaktivität ebenfalls entscheidend und sollte im warmen Bereich liegen. [2]

*   **Luftfeuchtigkeit:** 
    - Eine hohe relative Luftfeuchtigkeit ist für die meisten tropischen Pflanzen essenziell, um die Transpiration über die Blätter zu minimieren. Werte zwischen 60 % und 80 % sind optimal. [2]

*   **Bodenfeuchte:**  
    - Der Boden sollte konstant feucht, aber niemals durchnässt sein. [4]

    - Staunässe führt schnell zu Wurzelfäule und muss unbedingt vermieden werden. [5]

    - Der Boden sollte zwischen den Gießvorgängen leicht antrocknen können. [5]

*   **Lichtzyklus:** 
    - Ein Lichtzyklus von 12 bis 14 Stunden pro Tag simuliert die Bedingungen am Äquator und fördert ein gesundes vegetatives Wachstum. [6]

Quellen:

* [[1] hoklartherm.de](https://www.hoklartherm.de/ratgeber/tropische-pflanzen-gewaechshaus/)
* [[2] erlebnisgaertnerei.com](https://erlebnisgaertnerei.com/die-richtigen-temperaturen-im-gewaechshaus/)
* [[3] samen.de](https://samen.de/blog/temperaturmanagement-in-der-pflanzenanzucht-fuer-gesundes-pflanzenwachstum.html) 
* [[4] palmenstadl.de](https://palmenstadl.de/Die-richtige-Luftfeuchtigkeit-fuer-mediterrane-tropische-und-Wuestenpflanzen)  
* [[5] urban-jungle.store](https://www.urban-jungle.store/wie-giesse-ich-zimmerpflanzen-richtig/) 
* [[6] harmony-plants.com](https://www.harmony-plants.com/blogs/pflanzenmagazin/topf-und-bodenauswahl-fuer-tropische-pflanzen)

Damit lege ich folgende Sollwerte als Voreinstellung fest (diese können per Webinterface verändert werden): 

*   **`SOIL_TEMPERATUR_TARGET`**: `24.0` °C
*   **`SOIL_MOISTURE_TARGET`**: `50` % (Ein guter Mittelwert, der Staunässe vermeidet)
*   **`AIR_TEMPERATUR_THRESHOLD_HIGH`**: `28.0` °C (Ab hier wird es kritisch, Kühlung ist nötig)
*   **`HUMIDITY_TARGET`**: `70.0` %
*   **`LIGHT_ON_HOUR`**: `6` (6:00 Uhr)
*   **`LIGHT_OFF_HOUR`**: `20` (20:00 Uhr, ergibt einen 14-Stunden-Tag)

### Funktionsumfang

* **Bootvorgang:**  
  Nach dem Start wird eine Funktionsprüfung durchgeführt. Die Ergebnisse scrollen durch das Display. Wenn alles funktioniert, wird ein Splash-Screen (mein Portrait) für ein paar Sekunden angezeigt. Danach wird das Dashboard angezeigt. 

* **Dashboard:**  
  Das Dashboard teilt den Bildschirm in mehreren Bereichen auf. In jedem Bereich werden zusammenhängende Messwerte angezeigt. 

  Die Werte im Dashboard werden mit passenden Icons versehen:

  | Icon (XMB)     |                                                                | Verwendung                                                                       |
  |----------------|----------------------------------------------------------------|----------------------------------------------------------------------------------|
  | thermometer    | ![thermometer](./assets/icons/thermometer_16x16.xbm.png)       | Icon für Raumtemperatur (S1)                                                     |
  | wet            | ![wet](./assets/icons/wet_16x16.xbm.png)                       | Icon für Luftfeuchtigkeit (S1)                                                   |
  | engine_coolant | ![engine_coolant](./assets/icons/engine_coolant_16x16.xbm.png) | Icon für Bodentemperatur (S2)                                                    |
  | moisture       | ![moisture](./assets/icons/moisture_16x16.xbm.png)             | Icon für Bodenfeuchte (S3)                                                       |
  | radiator       | ![radiator](./assets/icons/radiator_16x16.xbm.png)             | Heizung (A3) aktiviert (überlagert Icon für Bodentemperatur)                     |
  | air            | ![air](./assets/icons/air_16x16.xbm.png)                       | Lüfter (A4) aktiviert (überlagert Icons für Raumtemperatur und Luftfeuchtigkeit) |
  | rainy_weather  | ![rainy_weather](./assets/icons/rainy_weather_16x16.xbm.png)   | Pumpe (A5) aktiviert (überlagert Icon für Bodenfeuchte)                          |
  | dry            | ![dry](./assets/icons/dry_16x16.xbm.png)                       | Vernebler (A6) aktiviert (überlagert Icon für Bodenfeuchte)                      |
  
  Die ursprünglichen Icons habe ich bei [Icons8](https://icons8.com/icons/all--mono--size-small--technique-line) gefunden und in `data/icons` gespeichert (wo sie für das Webinterface verfügbar sind).  
  Diese unterliegen der [Universal Multimedia License von Icons8](https://intercom.help/icons8-7fb7577e8170/en/articles/5534926-universal-multimedia-license-agreement-for-icons8).
  Demnach dürfen sie verwendet werden, solange das Projekt nicht-kommerziell ist. 
  
  Die Icons habe ich mit GIMP in das XMB-Format konvertiert und im `Include`-Verzeichnis gespeichert, sodass sie auf dem OLED-Display angezeigt werden können.

* **Warnung bei niedrigem Wasserstand:**  
  Bevor der Wasserreservoir (Teich) trocken läuft, soll nicht das Dashboard, sondern eine blinkende Warnung auf dem Display angezeigt werden (gern mit Icon). Über das Handy soll der Benutzer auch gewarnt werden (per E-Mail?).

* **Bild aufnehmen:**  
  Nach einem festgelegten Intervall (alle x Stunden) wird ein Bild geschossen und auf der SD-Karte gespeichert. Direkt danach wird ein weiteres Bild mit niedrigster Auflösung erzeugt und auf dem Display für ein paar Sekunden angezeigt, um zu zeigen, dass ein Bild gespeichert wurde. 

* **Webinterface:**  
  Das Webinterface bietet folgende Funktionen für das Handy an:
  * Als Startseite wird das Dashboard angezeigt. 
  * Auf einer anderen Seite (bzw. Kartenreiter) können die Parameter für die Automatisierung verändert werden.
  *	Die Aktoren können manuell ein- und ausgeschaltet werden (wobei die Automatisierung dann außer Kraft gesetzt wird).
  * Außerdem gibt es eine Gallery mit den aufgenommenen Bilder und die Möglichkeit, diese als Zeitraffer-Video abzuspielen. 
  * Eine weitere Funktion zeigt Liniendiagramme mit den Messwerten.

## Anhang

### Verwendete ASCII-Schaltzeichen 

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
* Z: Sonstige Peripherie
* R: Widerstand (Resistor)
* C: Kondensator/Elko (Capacitor)
* P: Stromversorgung (Power Supply)
* Q: Transistor
* D: Diode
* H: Lampe
* J: Anschluss (Stiftleiste, Buchsenleiste, DC-Hohlbuchse,...) 

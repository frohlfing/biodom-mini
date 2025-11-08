# ESP32

## GPIOs

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
* **(Boot):** GPIO 0, 2, 5, 12, 15 sind "Strapping-Pins". Besser nicht verwenden, denn der Bootvorgang wird hiermit beeinflusst. Könnten aber zur Not als digitale Ein- und Ausgänge verwenden werden (zu den Bedingungen siehe nachfolgende Tabelle). 
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

### Strapping-Pins

| Pin     | Funktion             | Bedingung                                  |
|---------|----------------------|--------------------------------------------|
| GPIO 0  | Boot-Modus           | Der Pin darf beim Booten nicht LOW sein.   | 
| GPIO 2  | Boot-Modus & Logging | Der Pin darf beim Booten nicht LOW sein.   | 
| GPIO 5  | Flash-Spannung       | Der Pin darf beim Booten nicht HIGH sein.  | 
| GPIO 12 | Flash-Spannung       | Der Pin darf beim Booten nicht HIGH sein.  | 
| GPIO 15 | Boot-Logging         | Der Pin darf beim Booten nicht LOW sein.   | 

## Hinweise zur Stromversorgung

### Schottky-Diode

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

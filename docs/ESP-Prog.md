# ESP-Prog

ESP-Prog ist ein Debugging-Tool von Espressif, basierend auf dem FT2232HL-Chip.

![ESP-Prog](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/_images/modules.png)  
[Benutzerhandbuch](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/esp-prog/user_guide.html)  
[H/W-Referenz](https://docs.espressif.com/projects/esp-dev-kits/en/latest/other/esp-prog/reference/reference-doc.html)  
[Erste Schritte mit PlatformIO](https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html)

## Schnittstellen

Der ESP-Prog stellt über ein einziges USB-Kabel zwei Schnittstellen bereit (per FTDI FT2232HL Chip):

* Interface 0 (JTAG-Schnittstelle): für Upload und Debugging via Standard-Debugging-Tool OpenOCD
* Interface 1 (UART-Schnittstelle): für Serielle Kommunikation (Serial Monitor/Plotter) via esptool.py

**Anmerkung zur Upload-Funktion:**

Das Hochladen des Codes kann auch so wie in der Arduino-IDE mittels esptool.py erfolgen (also über die UART-Schnittstelle). 
Dazu muss in der platformio.ini "upload_protocol = esptool" eingetragen werden (oder ausklammern, da das der Standardwert ist).
Aber diese Möglichkeit ziehe ich nicht weiter in Betracht.

**Kommunikationswege:**

* Upload und Debug: PC -> USB -> ESP-Prog -> Interface 0 (JTAG) -> ESP32
* Serieller Monitor: ESP32 -> Interface 1 (UART) -> ESP-Prog -> USB -> PC

## Treiber

**Erforderliche Treiber unter Windows:**

* Für **Interface 0** muss der FTDI-Treiber durch einen WinUSB-Treiber ersetzt werden, 
weil OpenOCD nicht direkt mit dem FTDI-Treiber kommunizieren kann.

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

## Buchsen

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

**Anschlusskabel:**

* Flachbandkabel (Ribbon Cable, meist 1.27mm oder 2.54mm Pitch)
* IDC-Stecker (2×3 und 2×5, Pfostenverbinder mit Schneidklemmen, auch Wannenstecker genannt)

Die Klemmen kann man mittels Schraubstock zusammenpressen.

## Stromversorgung

Der ESP-Prog wird über das USB-Kabel versorgt. Er kann den ESP32 mitversorgen, indem der 3.3V-Pin verbunden wird. 
Wenn aber eine andere Stromquelle für den ESP32 angeschlossen ist, darf der 3.3V-Pin nicht verbunden werden!

**Der PWR-SEL-Jumper muss auf 3.3V gesetzt sein!**

Auf 5V darf der Jumper nicht gesteckt werden, da sonst an einigen Pins mehr als 3.3 V liegen und der ESP32 darauf verschnupft reagieren könnte.

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

## Troubleshooting

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

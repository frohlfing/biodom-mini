/**
 * @file OLEDDisplaySH1106.h
 * OLEDDisplaySH1106 - schlanker Wrapper um U8g2 (olikraus)
 *
 * Ziel: Einfaches, gut dokumentiertes API für ESP32-Projekte, das typische
 * Display-Workflows (Text, Icons, Progress, Bitmap, invert) kapselt und
 * gleichzeitig Zugriff auf die rohe U8g2-Instanz erlaubt.
 *
 * Backend: U8g2 (https://github.com/olikraus/u8g2)
 *
 * Usage:
 * - Wähle passenden Konstruktor (SPI/I2C/Hardware) je nach Modul.
 * - rufe begin() in setup() auf.
 * - nutze print/printf/drawBitmap/streaming für Bilder.
 *
 * Lizenz: MIT
 */

#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

/**
 * OLEDDisplaySH1106
 *
 * Wrapperklasse um U8g2. Die Klasse verwaltet die U8g2-Instanz intern und
 * bietet kleine Hilfsfunktionen für häufige UI‑Elemente.
 */
class OLEDDisplaySH1106 {
public:
  /**
   * Konstruktor für SPI-Module (beispielhaft für SH1106/SSD1306 SPI).
   *
   * @param u8g2 Pointer auf eine fertig konstruierte U8G2-Instanz (empfohlen).
   *              Die Klasse übernimmt keinen Besitz; Instanz muss outlive this.
   * @note Du kannst auch direkt eine U8G2-Instanz im Sketch erzeugen und hier übergeben.
   */
  explicit OLEDDisplaySH1106(U8G2 *u8g2);

  /**
   * Initialisiert das Display (u8g2->begin()) und setzt Standardwerte.
   * Muss in setup() aufgerufen werden.
   *
   * @return true wenn erfolgreich initialisiert wurde, false andernfalls.
   */
  bool begin();

  /**
   * Löscht den Bildschirm-Buffer (u8g2.clearBuffer()).
   */
  void clear();

  /**
   * Zeichnet den internen Buffer auf das Display (u8g2.sendBuffer()).
   */
  void display();

  /**
   * Schreibt einfachen Text an Position (x,y) mit gewählter Font-Größe.
   *
   * @param x X-Pixel-Position (links = 0)
   * @param y Y-Pixel-Position (Baseline für u8g2.drawStr)
   * @param text Null-terminierter C-String
   */
  void drawText(int x, int y, const char* text);

  /**
   * Zeichnet Text mit printf-ähnlichem Format an Position (x,y).
   *
   * @param x X-Pixel-Position
   * @param y Y-Pixel-Position
   * @param fmt printf-format string
   * @param ... format args
   */
  void drawTextf(int x, int y, const char* fmt, ...);

  /**
   * Zeichnet eine horizontale Fortschrittsanzeige.
   *
   * @param x linke X-Position
   * @param y Top-Y-Position
   * @param w Breite (Pixel)
   * @param h Höhe (Pixel)
   * @param value 0..maxValue
   * @param maxValue Maximaler Wert
   * @param border true => Rahmen zeichnen
   */
  void drawProgress(int x, int y, int w, int h, uint32_t value, uint32_t maxValue, bool border = true);

  /**
   * Zeichnet ein 1-bit Bitmap (B/W).
   *
   * @param x linke X-Position
   * @param y Top-Y-Position
   * @param w Breite in Pixel
   * @param h Höhe in Pixel
   * @param bitmap Pointer auf uint8_t-Array im U8g2-Format (MSB-first)
   */
  void drawBitmap(int x, int y, int w, int h, const uint8_t* bitmap);

  /**
   * Invertiert Anzeige (weiß auf schwarz vs schwarz auf weiß).
   * @param inverted true => invertiert, false => normal
   */
  void setInverted(bool inverted);

  /**
   * Setzt Helligkeit / Kontrast (sofern der Treiber es unterstützt).
   *
   * @param contrast 0..255 (Werte außerhalb werden je nach Treiber geclamped)
   */
  void setContrast(uint8_t contrast);

  /**
   * Dreht das Display (0, 90, 180, 270). Führt u8g2.setDisplayRotation aus.
   *
   * @param degrees Rotation in Grad (nur 0, 90, 180, 270 unterstützt)
   * @return true bei akzeptiertem Wert, false bei ungültigem Wert
   */
  bool setRotation(uint16_t degrees);

  /**
   * Liefert rohen Zeiger auf die U8g2-Instanz, falls du Spezialbefehle brauchst.
   * Du darfst direkt u8g2 Methoden aufrufen (z. B. setFont, drawBox etc.).
   *
   * @return U8G2*-Pointer (kann nullptr sein, falls nicht initialisiert)
   */
  U8G2* u8g2();

  /**
   * Deinitialisiert die intern verwendeten Ressourcen (z. B. Buffer).
   * Optional: ruft u8g2->~U8G2 nicht; Instanz bleibt dem Aufrufer.
   */
  void end();

private:
  U8G2* _u8g2;
  bool _inverted;
};

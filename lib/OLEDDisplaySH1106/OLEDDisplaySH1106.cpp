/**
 * @file OLEDDisplaySH1106.cpp
 * Implementierung des OLEDDisplaySH1106 Wrappers.
 */

#include "OLEDDisplaySH1106.h"
#include <stdarg.h>

OLEDDisplaySH1106::OLEDDisplaySH1106(U8G2 *u8g2)
  : _u8g2(u8g2), _inverted(false) {}

bool OLEDDisplaySH1106::begin() {
  if (!_u8g2) return false;
  _u8g2->begin();
  // Standard-Font und Clear
  _u8g2->setFont(u8g2_font_ncenB08_tr);
  _u8g2->clearBuffer();
  _u8g2->sendBuffer();
  _inverted = false;
  return true;
}

void OLEDDisplaySH1106::clear() {
  if (!_u8g2) return;
  _u8g2->clearBuffer();
}

void OLEDDisplaySH1106::display() {
  if (!_u8g2) return;
  if (_inverted) {
    _u8g2->setDrawColor(0); // inverted: draw color 0 on inverted background
    // For inversion we flip with sendBuffer + display flip command if available.
    // Simpler: let caller flip by setInverted(); here we draw buffer normally.
  } else {
    _u8g2->setDrawColor(1);
  }
  _u8g2->sendBuffer();
}

void OLEDDisplaySH1106::drawText(int x, int y, const char* text) {
  if (!_u8g2 || !text) return;
  _u8g2->drawStr(x, y, text);
}

void OLEDDisplaySH1106::drawTextf(int x, int y, const char* fmt, ...) {
  if (!_u8g2 || !fmt) return;
  char buf[128];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  _u8g2->drawStr(x, y, buf);
}

void OLEDDisplaySH1106::drawProgress(int x, int y, int w, int h, uint32_t value, uint32_t maxValue, bool border) {
  if (!_u8g2 || w <= 0 || h <= 0) return;
  if (maxValue == 0) maxValue = 1;
  uint32_t clamped = value > maxValue ? maxValue : value;
  int filled = (int)((uint64_t)clamped * (uint32_t)w / maxValue);
  if (border) {
    _u8g2->drawFrame(x, y, w, h);
    if (filled > 0) _u8g2->drawBox(x+1, y+1, filled-2 > 0 ? filled-2 : 0, h-2 > 0 ? h-2 : 0);
  } else {
    if (filled > 0) _u8g2->drawBox(x, y, filled, h);
  }
}

void OLEDDisplaySH1106::drawBitmap(int x, int y, int w, int h, const uint8_t* bitmap) {
  if (!_u8g2 || !bitmap) return;
  _u8g2->drawXBMP(x, y, w, h, bitmap);
}

void OLEDDisplaySH1106::setInverted(bool inverted) {
  _inverted = inverted;
  if (!_u8g2) return;
  // U8g2 bietet sendBuffer + setDrawColor. True hardware inversion may require command:
  // Many OLED controllers support display invert via command; U8g2 has setDrawColor only.
  // Common approach: clear and redraw with inverted draw color or use setContrast if supported.
  _u8g2->clearBuffer();
  _u8g2->setDrawColor(inverted ? 0 : 1);
  _u8g2->sendBuffer();
}

void OLEDDisplaySH1106::setContrast(uint8_t contrast) {
  if (!_u8g2) return;
  // U8g2 exposes setContrast only via u8x8 or lower-level; try u8g2.setContrast
  // If unsupported on some controllers this is a no-op.
#ifdef U8X8_WITH_SET_CONTRAST
  _u8g2->setContrast(contrast);
#else
  // Fallback: no-op
  (void)contrast;
#endif
}

bool OLEDDisplaySH1106::setRotation(uint16_t degrees) {
  if (!_u8g2) return false;
  const u8g2_cb_t* rot;
  switch (degrees) {
    case 0:   rot = U8G2_R0; break;
    case 90:  rot = U8G2_R1; break;
    case 180: rot = U8G2_R2; break;
    case 270: rot = U8G2_R3; break;
    default:  return false;
  }
  _u8g2->setDisplayRotation(rot);
  return true;
}

U8G2* OLEDDisplaySH1106::u8g2() {
  return _u8g2;
}

void OLEDDisplaySH1106::end() {
  // kein Eigentum an _u8g2 => keine Löschung. Nur Reset intern.
  _u8g2 = nullptr;
  _inverted = false;
}

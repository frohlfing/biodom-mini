#include "OLEDDisplaySH1106.h"

OLEDDisplaySH1106::OLEDDisplaySH1106(const uint8_t resetPin)
    : _u8g2(U8G2_R0, resetPin)
{
}

void OLEDDisplaySH1106::begin() {
    _u8g2.begin();
}

void OLEDDisplaySH1106::clear() {
    _currentMode = NONE;
    _u8g2.clearBuffer();
    _u8g2.sendBuffer();
}

void OLEDDisplaySH1106::update() {
    if (_currentMode == ALERT && _isBlinking) {
        // Blink-Logik: Alle 500ms den Sichtbarkeitsstatus wechseln
        if (millis() - _lastBlinkTime > 500) {
            _lastBlinkTime = millis();
            _alertVisible = !_alertVisible;
            _drawFullscreenAlert(); // Neuzeichnen mit geändertem Status
        }
    }
}

// --- Log-Modus ---

void OLEDDisplaySH1106::addLogLine(const String &text) {
    _currentMode = LOG;
    if (_logLineCount < LOG_MAX_LINES) {
        // Puffer hat noch Platz
        _logLines[_logLineCount++] = text;
    } else {
        // Puffer ist voll, alle Zeilen nach oben schieben
        for (int i = 0; i < LOG_MAX_LINES - 1; i++) {
            _logLines[i] = _logLines[i + 1];
        }
        _logLines[LOG_MAX_LINES - 1] = text;
    }
    _drawLog();
}

void OLEDDisplaySH1106::clearLog() {
    _logLineCount = 0;
    if (_currentMode == LOG) {
        clear();
    }
}

void OLEDDisplaySH1106::_drawLog() {
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_6x10_tf);
    _u8g2.setFontPosTop();

    // Bestimme, welche Zeilen gezeichnet werden sollen
    int startLine = max(0, _logLineCount - (int)LOG_VISIBLE_LINES);
    
    for (int i = 0; i < LOG_VISIBLE_LINES; i++) {
        if (startLine + i < _logLineCount) {
            _u8g2.drawStr(0, i * 10, _logLines[startLine + i].c_str());
        }
    }
    _u8g2.sendBuffer();
}

// --- Dashboard-Modus ---

void OLEDDisplaySH1106::setDashboardText(Quadrant q, const String &text) {
    _dashboardText[q] = text;
}

void OLEDDisplaySH1106::setDashboardIcon(Quadrant q, uint8_t width, uint8_t height, const uint8_t *icon) {
    _dashboardIcon[q] = icon;
    _dashboardIconWidth[q] = width;
    _dashboardIconHeight[q] = height;
}

void OLEDDisplaySH1106::showDashboard() {
    _currentMode = DASHBOARD;
    _drawDashboard();
}

void OLEDDisplaySH1106::_drawDashboard() {
    _u8g2.clearBuffer();
    
    // Trennlinien zeichnen
    _u8g2.drawLine(64, 0, 64, 63); // Vertikal
    _u8g2.drawLine(0, 32, 127, 32); // Horizontal

    // Quadranten zeichnen
    int quadrantWidth = 64;
    int quadrantHeight = 32;

    for (int q = 0; q < 4; q++) {
        int x_offset = (q % 2) * quadrantWidth;
        int y_offset = (q / 2) * quadrantHeight;

        _u8g2.setFont(u8g2_font_7x13_tf);
        _u8g2.setFontPosTop();

        // Icon zeichnen, falls vorhanden
        if (_dashboardIcon[q] != nullptr) {
            _u8g2.drawXBMP(x_offset + 2, y_offset + 2, _dashboardIconWidth[q], _dashboardIconHeight[q], _dashboardIcon[q]);
        }
        
        // Text zeichnen
        _u8g2.drawStr(x_offset + 2, y_offset + 18, _dashboardText[q].c_str());
    }
    
    _u8g2.sendBuffer();
}

// --- Alert-Modus ---

void OLEDDisplaySH1106::showFullscreenAlert(const String &message, bool blink) {
    _currentMode = ALERT;
    _alertMessage = message;
    _isBlinking = blink;
    _alertVisible = true; // Immer sichtbar beim ersten Aufruf
    _lastBlinkTime = millis();
    _drawFullscreenAlert();
}

void OLEDDisplaySH1106::_drawFullscreenAlert() {
    _u8g2.clearBuffer();
    if (_alertVisible) {
        _u8g2.setFont(u8g2_font_ncenB10_tr); // eine mittelgroße Schriftart
        _u8g2.setFontPosCenter(); // Text horizontal und vertikal zentrieren
        int newlineIndex = _alertMessage.indexOf('\n');
        if (newlineIndex == -1) { // eine Zeile
            // Text in der Mitte des Bildschirms ausrichten und anzeigen
            int textWidth = _u8g2.getStrWidth(_alertMessage.c_str());
            _u8g2.drawStr((128 - textWidth) / 2, 32, _alertMessage.c_str());
        } else { // zwei Zeilen
            String line1 = _alertMessage.substring(0, newlineIndex);
            int textWidth1 = _u8g2.getStrWidth(line1.c_str());
            _u8g2.drawStr((128 - textWidth1) / 2, 22, line1.c_str());
            String line2 = _alertMessage.substring(newlineIndex + 1);
            int textWidth2 = _u8g2.getStrWidth(line2.c_str());
            _u8g2.drawStr((128 - textWidth2) / 2, 42, line2.c_str());
        }
    }
   _u8g2.sendBuffer();
}
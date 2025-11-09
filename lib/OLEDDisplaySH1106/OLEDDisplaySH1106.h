#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

/**
 * Klasse für den 1.3 Zoll OLED Display SH1106.
 * 
 * Diese Klasse stellt eine High-Level-API zur Verfügung, um spezifische
 * UI-Muster auf dem Display darzustellen, wie z.B. einen scrollenden Log,
 * ein 4-Quadranten-Dashboard oder bildschirmfüllende Warnmeldungen.
 */
class OLEDDisplaySH1106
{
public:
    /**
     * @enum Quadrant
     * @brief Definiert die vier Quadranten des Dashboard-Modus.
     */
    enum Quadrant { TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT };

    /**
     * @brief Konstruktor.
     * @param resetPin Optionaler GPIO-Pin für den Reset des Displays.
     */
    OLEDDisplaySH1106(const uint8_t resetPin = U8X8_PIN_NONE);

    /**
     * @brief Initialisiert das Display.
     * Muss im setup() des Hauptprogramms aufgerufen werden.
     */
    void begin();

    /**
     * @brief Muss regelmäßig in der Hauptschleife (loop()) aufgerufen werden.
     * Steuert zeitbasierte Animationen wie z.B. das Blinken von Warnmeldungen.
     */
    void update();
    
    /**
     * @brief Löscht den gesamten Bildschirminhalt.
     */
    void clear();

    // --- Modus 1: Scrolling Log ---

    /**
     * @brief Fügt eine neue Zeile zum Log hinzu.
     * Wenn der Bildschirm voll ist, scrollt der vorhandene Text nach oben.
     * @param text Die hinzuzufügende Textzeile.
     */
    void addLogLine(const String &text);
    
    /**
     * @brief Löscht den Inhalt des Log-Modus.
     */
    void clearLog();

    // --- Modus 2: Dashboard ---

    /**
     * @brief Setzt den Text für einen bestimmten Quadranten im Dashboard.
     * @param q Der Quadrant, der aktualisiert werden soll.
     * @param text Der anzuzeigende Text (z.B. "23.5°C").
     */
    void setDashboardText(Quadrant q, const String &text);

    /**
     * @brief Setzt ein Icon für einen bestimmten Quadranten.
     * @param q Der Quadrant, der das Icon erhalten soll.
     * @param width Breite des Icons in Pixel.
     * @param height Höhe des Icons in Pixel.
     * @param icon Ein Pointer auf das Byte-Array des Icons (XBM-Format).
     */
    void setDashboardIcon(Quadrant q, uint8_t width, uint8_t height, const uint8_t *icon);

    /**
     * @brief Zeichnet das Dashboard mit den aktuell gesetzten Werten.
     */
    void showDashboard();

    // --- Modus 3: Fullscreen Alert ---
    
    /**
     * @brief Zeigt eine bildschirmfüllende Nachricht an.
     * @param message Die anzuzeigende Warnung.
     * @param blink Aktiviert das Blinken des Textes (erfordert regelmäßige `update()`-Aufrufe).
     */
    void showFullscreenAlert(const String &message, bool blink = false);


private:
    U8G2_SH1106_128X64_NONAME_F_HW_I2C _u8g2; // Die Instanz der U8g2-Grafikbibliothek, die das Display steuert.

    // --- Zustandsvariablen für den Log-Modus ---
    static const uint8_t LOG_MAX_LINES = 12;    // Die maximale Anzahl von Zeilen, die der interne Log-Puffer speichern kann.
    static const uint8_t LOG_VISIBLE_LINES = 6; // Die Anzahl der Log-Zeilen, die gleichzeitig auf dem Display sichtbar sind.
    String _logLines[LOG_MAX_LINES];            // Der Puffer, der die Textzeilen des Logs speichert.
    int _logLineCount = 0;                      // Der Puffer, der die Textzeilen des Logs speichert.
    int _logScrollOffset = 0;                   // Die aktuelle Anzahl der im Puffer gespeicherten Log-Zeilen.
    
    /**
     * @brief Interne Funktion zum Zeichnen des Log-Inhalts in den Display-Puffer.
     */
    void _drawLog();

    // --- Zustandsvariablen für das Dashboard ---
    String _dashboardText[4];         // Ein Array, das die Texte für die vier Quadranten speichert.
    const uint8_t* _dashboardIcon[4] = {nullptr, nullptr, nullptr, nullptr}; // Ein Array, das die Pointer auf die Icon-Bitmaps der vier Quadranten speichert.
    uint8_t _dashboardIconWidth[4];   // Speichert die Breite der Icons für die korrekte Positionierung.
    uint8_t _dashboardIconHeight[4];  // Speichert die Höhe der Icons für die korrekte Positionierung.
    
    /**
     * @brief Interne Funktion zum Zeichnen des Dashboards in den Display-Puffer.
     */
    void _drawDashboard();

    // --- Zustandsvariablen für den Alert-Modus ---
    String _alertMessage;             // Speichert die anzuzeigende Warnmeldung.
    bool _isBlinking = false;         // Flag, das steuert, ob die Warnmeldung blinken soll.
    bool _alertVisible = true;        // Internes Flag für den Blink-Effekt (an/aus).
    unsigned long _lastBlinkTime = 0; // Zeitstempel des letzten Blink-Zustandswechsels.
    
    /**
     * @brief Interne Funktion zum Zeichnen der Warnmeldung in den Display-Puffer.
     * Die Warnmeldung kann mit einem Zeilenumbruch ('\n') auf zwei Zeilen aufgeteilt werden.
     */
    void _drawFullscreenAlert();

    // Aktiver Anzeigemodus
    enum DisplayMode { NONE, LOG, DASHBOARD, ALERT }; // Definiert den aktuellen Darstellungsmodus des Displays.
    DisplayMode _currentMode = NONE;                  // Speichert den aktuell aktiven Anzeigemodus.
};
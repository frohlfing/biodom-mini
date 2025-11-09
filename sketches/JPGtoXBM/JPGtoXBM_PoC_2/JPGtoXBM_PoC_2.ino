// JPGtoXBM_PoC_2.ino
// PoC2: TJpg_Decoder (Bodmer) + Floyd-Steinberg Dithering (optional) -> XBM Buffer -> u8g2 SH1106
// Bit-Layout: xbm_buffer, 1 bit per pixel, bit==1 -> black pixel

#include <Arduino.h>
#include <U8g2lib.h>
#include <TJpg_Decoder.h>

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// ==================== Konfiguration ====================
#define USE_TEST_PATTERN     false   // true = einfacher horizontaler Strich, false = echtes JPG aus frank_128x64_clean_jpg.h
#define USE_DITHERING        true    // true = Floyd–Steinberg Dithering, false = simples Thresholding
#define USE_NEGATIVE_DISPLAY true    // true = Negativ auf OLED (weißer Hintergrund, "stanzen" der schwarzen Pixel)

// Display / Bildgrößen
#define XBM_WIDTH  128
#define XBM_HEIGHT 64

// Include JPG-Array (wenn realer Modus)
#if !USE_TEST_PATTERN
  #include "frank_128x64_clean_jpg.h" // erwartet: static const unsigned char image_jpg[] = { ... };
#endif

// ==================== Hardware: U8g2 SH1106 I2C ====================
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// ==================== XBM-Puffer ====================
// 1 Bit / Pixel -> (128*64)/8 = 1024 Bytes
static uint8_t xbm_buffer[XBM_WIDTH * XBM_HEIGHT / 8];

// Set/clear Bit: bit==1 => black pixel
inline void xbm_set_pixel(int x, int y, bool black) {
  if (x < 0 || x >= XBM_WIDTH || y < 0 || y >= XBM_HEIGHT) return;
  int byteIndex = (y * XBM_WIDTH + x) >> 3; // /8
  int bitIndex  = x & 7;                    // %8
  if (black) xbm_buffer[byteIndex] |= (1 << bitIndex);
  else       xbm_buffer[byteIndex] &= ~(1 << bitIndex);
}

// ==================== Dither-Fehlerpuffer (Floyd–Steinberg) ====================
// Wir nutzen zwei Zeilen-Puffer mit je XBM_WIDTH + 2 Rand-Elementen.
// Fehler werden als int16 gespeichert und in Integer-Arithmetik mit Skalierung 16 gehandhabt.
static int16_t error_line1[XBM_WIDTH + 2];
static int16_t error_line2[XBM_WIDTH + 2];

static void clear_error_lines() {
  memset(error_line1, 0, sizeof(error_line1));
  memset(error_line2, 0, sizeof(error_line2));
}

// ==================== Test-Muster (schnelle Validierung) ====================
static void render_test_pattern() {
  memset(xbm_buffer, 0, sizeof(xbm_buffer)); // 0 = white (keine schwarzen Pixel)
  // horizontaler Strich in der Mitte
  int y = XBM_HEIGHT / 2;
  for (int x = 0; x < XBM_WIDTH; ++x) {
    xbm_set_pixel(x, y, true);
  }
}

// ==================== TJpg_Decoder Render-Callback ====================
// Signatur für Bodmers TJpg_Decoder: void callback(uint16_t *pPixels, int x, int y, int w, int h)
void jpgRender(uint16_t *pPixels, int x, int y, int w, int h) {
  // Clip-Blöcke, die komplett außerhalb liegen
  if (y >= XBM_HEIGHT || x >= XBM_WIDTH) return;

  for (int yy = 0; yy < h; ++yy) {
    int ty = y + yy;
    if (ty < 0 || ty >= XBM_HEIGHT) continue;

    // Wähle die aktuellen Fehlerzeilen
    int16_t* cur = (ty % 2 == 0) ? error_line1 : error_line2;
    int16_t* nxt = (ty % 2 == 0) ? error_line2 : error_line1;

    // Wenn Block am Zeilenanfang startet, lösche die "next" Linie (Vorbereitung)
    if (x == 0) {
      memset(nxt, 0, sizeof(int16_t) * (XBM_WIDTH + 2));
    }

    for (int xx = 0; xx < w; ++xx) {
      int tx = x + xx;
      if (tx < 0 || tx >= XBM_WIDTH) { continue; }

      // RGB565 -> 8-bit Komponenten
      uint16_t pix = pPixels[yy * w + xx];
      uint8_t r5 = (pix >> 11) & 0x1F;
      uint8_t g6 = (pix >> 5) & 0x3F;
      uint8_t b5 = (pix >> 0) & 0x1F;
      uint8_t r8 = (r5 << 3) | (r5 >> 2);
      uint8_t g8 = (g6 << 2) | (g6 >> 4);
      uint8_t b8 = (b5 << 3) | (b5 >> 2);

      // Luminanz (0..255)
      int lum = (299 * r8 + 587 * g8 + 114 * b8) / 1000;

      // Fehler aus aktueller Zeile hinzufügen (Index mit +1 wegen Rand-Padding)
      int idx = tx + 1;
      int corrected = lum + (cur[idx] >> 4); // cur holds errors scaled by 16
      corrected = constrain(corrected, 0, 255);

      int out; // 0 = black, 255 = white (we invert later when setting bits)
      if (USE_DITHERING) {
        out = (corrected < 128) ? 0 : 255;
        int err = corrected - out; // -255..255

        // Fehler skalieren um 16 (Ganzzahlarithmetik)
        int e = err * 16;
        // right
        if (tx + 1 < XBM_WIDTH) cur[idx + 1] += (e * 7) / 16;
        // bottom-left
        if (ty + 1 < XBM_HEIGHT && tx - 1 >= 0) nxt[(tx - 1) + 1] += (e * 3) / 16;
        // bottom
        if (ty + 1 < XBM_HEIGHT) nxt[idx] += (e * 5) / 16;
        // bottom-right
        if (ty + 1 < XBM_HEIGHT && tx + 1 < XBM_WIDTH) nxt[(tx + 1) + 1] += (e * 1) / 16;
      } else {
        // simples Thresholding
        out = (corrected < 128) ? 0 : 255;
      }

      // Setze Pixel im XBM-Puffer: out==0 => black pixel => set bit 1
      bool black = (out == 0);
      xbm_set_pixel(tx, ty, black);

      // Clear current slot um Ringwrap zu verhindern
      cur[idx] = 0;
    } // for xx
  } // for yy
}

// ==================== Hauptablauf (setup) ====================
void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("JPGtoXBM PoC2 - TJpg_Decoder + Floyd-Steinberg (optional)");

  u8g2.begin();

  // Vorbereitungen
  memset(xbm_buffer, 0, sizeof(xbm_buffer)); // 0 = white
  clear_error_lines();

  if (USE_TEST_PATTERN) {
    Serial.println("Modus: TEST PATTERN");
    render_test_pattern();
  } else {
    Serial.println("Modus: echtes JPG, decode starte...");
    // decode aus Array mit Bodmers drawJpg API
    bool ok = TJpgDec.drawJpg((const uint8_t*)image_jpg, sizeof(image_jpg), jpgRender);
    if (!ok) {
      Serial.println("TJpgDec.drawJpg failed");
      // Fallback: markiere Fehler visuell
      for (int y = 28; y < 36; ++y)
        for (int x = 8; x < 120; ++x)
          xbm_set_pixel(x, y, true);
    } else {
      Serial.println("Decodierung erfolgreich.");
    }
  }

  // === Anzeige auf OLED ===
  // Wenn USE_NEGATIVE_DISPLAY==true: wir wollen weißen Hintergrund und "stanzen" schwarze Pixel.
  u8g2.clearBuffer();
  if (USE_NEGATIVE_DISPLAY) {
    u8g2.setDrawColor(1);           // DrawColor 1 = pixel ON (white)
    u8g2.drawBox(0, 0, XBM_WIDTH, XBM_HEIGHT);
    u8g2.setDrawColor(0);           // DrawColor 0 = pixel OFF (black) -> wir "löschen" weiße Pixel dort, wo 1 im XBM steht
  } else {
    // normale Anzeige: schwarzer Hintergrund, weiße Pixel für '1' -> invertiere beim draw.
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 0, XBM_WIDTH, XBM_HEIGHT); // fill black
    u8g2.setDrawColor(1); // white for bits
  }

  // u8g2.drawXBMP erwartet 1=black? U8g2's drawXBMP expects XBM format where 1 bits are black pixels.
  // Wir haben xbm_buffer so aufgebaut, dass 1 == black, also wir können drawXBMP direkt verwenden.
  u8g2.drawXBMP(0, 0, XBM_WIDTH, XBM_HEIGHT, xbm_buffer);
  u8g2.sendBuffer();

  Serial.println("Bild auf Display ausgegeben.");
}

// ==================== loop ====================
void loop() {
  // statisches Bild, nichts weiter zu tun
  delay(1000);
}

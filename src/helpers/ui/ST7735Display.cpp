#include "ST7735Display.h"
#include "EmbeddedBitmapFonts.h"
#include "Utf8Cyrillic5x7.h"

#ifndef DISPLAY_ROTATION
  #define DISPLAY_ROTATION 2
#endif

struct ST7735Theme {
  uint16_t fg;
  uint16_t bg;
  uint16_t red;
  uint16_t green;
  uint16_t blue;
  uint16_t yellow;
  uint16_t orange;
  const char* name;
};

static const ST7735Theme ST7735_THEMES[] = {
  {0xF7DF, 0x0000, 0xFA69, 0x1F8F, 0x365F, 0xFF09, 0xFCC5, "Графит"},
  {0xEFFF, 0x0008, 0xFB2C, 0x2F50, 0x4DFF, 0xFF6A, 0xFD60, "Полночь"},
  {0xEFFF, 0x0020, 0xFB4B, 0x27EF, 0x2EB7, 0xFF08, 0xFB82, "Хвоя"},
  {0x10C4, 0xFFDE, 0xD924, 0x0BC9, 0x0354, 0xB281, 0xC201, "Бумага"},
  {0xFF9F, 0x0001, 0xFA6F, 0x2F50, 0x653F, 0xFF49, 0xFB90, "Бордо"},
  {0xEFFF, 0x0022, 0xFB8E, 0x3693, 0x269D, 0xFF08, 0xFC87, "Север"},
  {0xFFFF, 0x0000, 0xF800, 0x07E0, 0x001F, 0xFFE0, 0xFD20, "Высокий"}
};

static uint16_t readDisplayCodepoint(const char*& str) {
  const char* start = str;
  uint8_t first = (uint8_t)*start;
  uint16_t cp = meshcoreReadUtf8Codepoint(str);
  if (cp == '?' && first >= 0x80 && str == start + 1) {
    uint16_t cp1251;
    if (meshcoreCp1251Codepoint(first, &cp1251)) return cp1251;
  }
  return cp;
}

bool ST7735Display::i2c_probe(TwoWire& wire, uint8_t addr) {
  return true;
/*
  wire.beginTransmission(addr);
  uint8_t error = wire.endTransmission();
  return (error == 0);
*/
}

bool ST7735Display::begin() {
  if (!_isOn) {
    if (_peripher_power) _peripher_power->claim();

    pinMode(PIN_TFT_LEDA_CTL, OUTPUT);
#if defined(PIN_TFT_LEDA_CTL_ACTIVE)
    digitalWrite(PIN_TFT_LEDA_CTL, PIN_TFT_LEDA_CTL_ACTIVE);
#else
    digitalWrite(PIN_TFT_LEDA_CTL, HIGH); 
#endif
    digitalWrite(PIN_TFT_RST, HIGH);

#if defined(HELTEC_TRACKER_V2) || defined(HELTEC_T096)
    display.initR(INITR_MINI160x80);
    display.setRotation(DISPLAY_ROTATION);
    uint8_t madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV |ST7735_MADCTL_BGR;//Adjust color to BGR
    display.sendCommand(ST77XX_MADCTL, &madctl, 1);
#else
    display.initR(INITR_MINI160x80_PLUGIN);
    display.setRotation(DISPLAY_ROTATION);
#endif
    display.setSPISpeed(40000000);
    applyTheme();
    display.fillScreen(_theme_bg);
    display.setTextColor(_theme_fg, _theme_bg);
    display.setTextSize(1);
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    
    _isOn = true;
  }
  return true;
}

void ST7735Display::turnOn() {
  ST7735Display::begin();
}

void ST7735Display::turnOff() {
  if (_isOn) {
    digitalWrite(PIN_TFT_RST, LOW);
#if defined(PIN_TFT_LEDA_CTL_ACTIVE)
    digitalWrite(PIN_TFT_LEDA_CTL, !PIN_TFT_LEDA_CTL_ACTIVE);
#else
    digitalWrite(PIN_TFT_LEDA_CTL, LOW); 
#endif
    _isOn = false;

    if (_peripher_power) _peripher_power->release();
  }
}

void ST7735Display::clear() {
  //Serial.println("DBG: display.Clear");
  if (_frame_active) {
    fillRectRaw(0, 0, width(), height(), _theme_bg);
  } else {
    display.fillScreen(_theme_bg);
  }
}

void ST7735Display::startFrame(Color bkg) {
  (void)bkg;
  applyTheme();
#if ST7735_USE_FRAMEBUFFER
  _frame_active = true;
  fillRectRaw(0, 0, width(), height(), _theme_bg);
#else
  _frame_active = false;
  display.fillScreen(_theme_bg);
#endif
  _color = _theme_fg;
  display.setTextColor(_color, _theme_bg);
  _text_size = 1;
  _bold_text = false;
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
}

void ST7735Display::setTextSize(int sz) {
  if (sz < 1) sz = 1;
  _text_size = (uint8_t)sz;
}

void ST7735Display::setBold(bool bold) {
  _bold_text = bold;
}

void ST7735Display::applyTheme() {
  uint8_t count = sizeof(ST7735_THEMES) / sizeof(ST7735_THEMES[0]);
  if (_ui_theme >= count) _ui_theme = 0;
  _theme_fg = ST7735_THEMES[_ui_theme].fg;
  _theme_bg = ST7735_THEMES[_ui_theme].bg;
}

const MeshcoreBitmapFont* ST7735Display::currentFont() const {
  return meshcoreGetSmallFont(_ui_font);
}

const MeshcoreBitmapGlyph* ST7735Display::glyphForCodepoint(uint16_t codepoint) const {
  const MeshcoreBitmapFont* font = currentFont();
  const MeshcoreBitmapGlyph* glyph = meshcoreFindGlyph(font, codepoint == '\t' ? ' ' : codepoint);
  if (glyph) return glyph;
  return meshcoreFindGlyph(font, '?');
}

uint8_t ST7735Display::fontLineHeight() const {
  return currentFont()->height * _text_size;
}

void ST7735Display::setUiFont(uint8_t font_id) {
  if (font_id >= getUiFontCount()) font_id = 0;
  _ui_font = font_id;
}

const char* ST7735Display::getUiFontName(uint8_t font_id) const {
  if (font_id >= getUiFontCount()) font_id = 0;
  return meshcoreGetSmallFont(font_id)->name;
}

uint8_t ST7735Display::getUiFontCount() const {
  return MESHCORE_SMALL_FONT_COUNT;
}

void ST7735Display::setUiTheme(uint8_t theme_id) {
  if (theme_id >= getUiThemeCount()) theme_id = 0;
  _ui_theme = theme_id;
  applyTheme();
}

const char* ST7735Display::getUiThemeName(uint8_t theme_id) const {
  if (theme_id >= getUiThemeCount()) theme_id = 0;
  return ST7735_THEMES[theme_id].name;
}

void ST7735Display::setColor(Color c) {
  const ST7735Theme& theme = ST7735_THEMES[_ui_theme < getUiThemeCount() ? _ui_theme : 0];
  switch (c) {
    case DisplayDriver::DARK :
      _color = _theme_bg;
      break;
    case DisplayDriver::RED : 
      _color = theme.red;
      break;
    case DisplayDriver::GREEN : 
      _color = theme.green;
      break;
    case DisplayDriver::BLUE : 
      _color = theme.blue;
      break;
    case DisplayDriver::YELLOW : 
      _color = theme.yellow;
      break;
    case DisplayDriver::ORANGE : 
      _color = theme.orange;
      break;
    default:
      _color = _theme_fg;
      break;
  }
  display.setTextColor(_color, _theme_bg);
}

void ST7735Display::setCursor(int x, int y) {
  _cursor_x = x;
  _cursor_y = y;
  display.setCursor(x, y);
}

uint16_t ST7735Display::codepointWidth(uint16_t codepoint) {
  if (codepoint == '\r' || codepoint == '\n') return 0;
  const MeshcoreBitmapGlyph* glyph = glyphForCodepoint(codepoint);
  if (!glyph) return 0;
  uint16_t extra = (_bold_text && _text_size == 1) ? 1 : 0;
  return glyph->xAdvance * _text_size + extra;
}

void ST7735Display::writePixel(int x, int y, uint16_t color) {
  if (x < 0 || y < 0 || x >= width() || y >= height()) return;
#if ST7735_USE_FRAMEBUFFER
  if (_frame_active) {
    _framebuffer[y * width() + x] = color;
  } else {
    display.drawPixel(x, y, color);
  }
#else
  display.drawPixel(x, y, color);
#endif
}

void ST7735Display::fillRectRaw(int x, int y, int w, int h, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  if (x < 0) {
    w += x;
    x = 0;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (x >= width() || y >= height()) return;
  if (x + w > width()) w = width() - x;
  if (y + h > height()) h = height() - y;
  if (w <= 0 || h <= 0) return;

#if ST7735_USE_FRAMEBUFFER
  if (_frame_active) {
    for (int row = 0; row < h; row++) {
      uint16_t* dst = &_framebuffer[(y + row) * width() + x];
      for (int col = 0; col < w; col++) dst[col] = color;
    }
  } else {
    display.fillRect(x, y, w, h, color);
  }
#else
  display.fillRect(x, y, w, h, color);
#endif
}

void ST7735Display::drawRectRaw(int x, int y, int w, int h, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  fillRectRaw(x, y, w, 1, color);
  fillRectRaw(x, y + h - 1, w, 1, color);
  fillRectRaw(x, y, 1, h, color);
  fillRectRaw(x + w - 1, y, 1, h, color);
}

void ST7735Display::drawCodepoint(uint16_t codepoint) {
  if (codepoint == '\r') return;
  if (codepoint == '\n') {
    setCursor(0, _cursor_y + fontLineHeight());
    return;
  }

  const MeshcoreBitmapFont* font = currentFont();
  const MeshcoreBitmapGlyph* glyph = glyphForCodepoint(codepoint);
  if (!glyph) return;

  int baseline = _cursor_y + font->ascent * _text_size;
  int top = baseline - (glyph->yOffset + glyph->height) * _text_size;
  int left = _cursor_x + glyph->xOffset * _text_size;
  bool bold = _bold_text && _text_size == 1;

  for (int row = 0; row < glyph->height; row++) {
    int run_start = -1;
    for (int col = 0; col <= glyph->width; col++) {
      bool set = false;
      if (col < glyph->width) {
        uint8_t bits = font->bitmap[glyph->offset + row * glyph->rowBytes + col / 8];
        set = (bits & (1 << (col & 7))) != 0;
      }
      if (set) {
        if (run_start < 0) run_start = col;
      } else if (run_start >= 0) {
        int px = left + run_start * _text_size;
        int py = top + row * _text_size;
        int run_w = (col - run_start) * _text_size + (bold ? 1 : 0);
        fillRectRaw(px, py, run_w, _text_size, _color);
        run_start = -1;
      }
    }
  }

  _cursor_x += codepointWidth(codepoint);
  display.setCursor(_cursor_x, _cursor_y);
}

void ST7735Display::print(const char* str) {
  if (str == NULL) return;
  while (*str) {
    drawCodepoint(readDisplayCodepoint(str));
  }
}

void ST7735Display::printWordWrap(const char* str, int max_width) {
  if (str == NULL) return;
  int left = _cursor_x;
  int right = left + max_width;
  while (*str) {
    uint16_t cp = readDisplayCodepoint(str);
    uint16_t w = codepointWidth(cp);
    if (cp == '\n') {
      drawCodepoint(cp);
      setCursor(left, _cursor_y);
      continue;
    }
    if (w > 0 && _cursor_x + w > right && _cursor_x > left) {
      setCursor(left, _cursor_y + fontLineHeight());
      if (cp == ' ') continue;
    }
    drawCodepoint(cp);
  }
}

void ST7735Display::translateUTF8ToBlocks(char* dest, const char* src, size_t dest_size) {
  meshcoreCopySupportedUtf8(dest, src, dest_size);
}

void ST7735Display::fillRect(int x, int y, int w, int h) {
  fillRectRaw(x, y, w, h, _color);
}

void ST7735Display::drawRect(int x, int y, int w, int h) {
  drawRectRaw(x, y, w, h, _color);
}

void ST7735Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  uint8_t byteWidth = (w + 7) / 8;
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      uint8_t byte = pgm_read_byte(bits + j * byteWidth + i / 8);
      if (byte & (0x80 >> (i & 7))) {
        writePixel(x + i, y + j, _color);
      }
    }
  }
}

uint16_t ST7735Display::getTextWidth(const char* str) {
  if (str == NULL) return 0;
  uint16_t w = 0;
  uint16_t max_w = 0;
  while (*str) {
    uint16_t cp = readDisplayCodepoint(str);
    if (cp == '\n') {
      if (w > max_w) max_w = w;
      w = 0;
    } else {
      w += codepointWidth(cp);
    }
  }
  return w > max_w ? w : max_w;
}

void ST7735Display::endFrame() {
#if ST7735_USE_FRAMEBUFFER
  if (!_frame_active) return;
  if (_isOn) {
    display.startWrite();
    display.drawRGBBitmap(0, 0, _framebuffer, width(), height());
    display.endWrite();
  }
  _frame_active = false;
#else
  _frame_active = false;
#endif
}

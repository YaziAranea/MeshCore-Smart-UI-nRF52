#pragma once

#include "DisplayDriver.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <helpers/RefCountedDigitalPin.h>

#ifndef ST7735_USE_FRAMEBUFFER
  #define ST7735_USE_FRAMEBUFFER 1
#endif

class ST7735Display : public DisplayDriver {
  Adafruit_ST7735 display;
  bool _isOn;
  uint16_t _color = ST77XX_WHITE;
  RefCountedDigitalPin* _peripher_power;
  int _cursor_x = 0;
  int _cursor_y = 0;
  uint8_t _text_size = 1;
  bool _bold_text = false;
  uint8_t _ui_font = 0;
  uint8_t _ui_theme = 0;
  uint16_t _theme_fg = ST77XX_WHITE;
  uint16_t _theme_bg = ST77XX_BLACK;
#if ST7735_USE_FRAMEBUFFER
  uint16_t _framebuffer[160 * 80];
#endif
  bool _frame_active = false;

  bool i2c_probe(TwoWire& wire, uint8_t addr);
  void applyTheme();
  const struct MeshcoreBitmapFont* currentFont() const;
  const struct MeshcoreBitmapGlyph* glyphForCodepoint(uint16_t codepoint) const;
  uint8_t fontLineHeight() const;
  uint16_t codepointWidth(uint16_t codepoint);
  void writePixel(int x, int y, uint16_t color);
  void fillRectRaw(int x, int y, int w, int h, uint16_t color);
  void drawRectRaw(int x, int y, int w, int h, uint16_t color);
  void drawCodepoint(uint16_t codepoint);
public:
#ifdef USE_PIN_TFT
  ST7735Display(RefCountedDigitalPin* peripher_power=NULL) : DisplayDriver(160, 80),
      display(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_SDA, PIN_TFT_SCL, PIN_TFT_RST),
      _peripher_power(peripher_power)
  {
    _isOn = false;
  }
#else
  ST7735Display(RefCountedDigitalPin* peripher_power=NULL) : DisplayDriver(160, 80),
      display(&SPI1, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST),
      _peripher_power(peripher_power)
  {
    _isOn = false;
  }
#endif
  bool begin();

  bool isOn() override { return _isOn; }
  void turnOn() override;
  void turnOff() override;
  void clear() override;
  void startFrame(Color bkg = DARK) override;
  void setTextSize(int sz) override;
  void setBold(bool bold) override;
  uint8_t getTextLineHeight() const override { return fontLineHeight(); }
  void setUiFont(uint8_t font_id) override;
  uint8_t getUiFont() const override { return _ui_font; }
  uint8_t getUiFontCount() const override;
  const char* getUiFontName(uint8_t font_id) const override;
  void setUiTheme(uint8_t theme_id) override;
  uint8_t getUiTheme() const override { return _ui_theme; }
  uint8_t getUiThemeCount() const override { return 7; }
  const char* getUiThemeName(uint8_t theme_id) const override;
  void setColor(Color c) override;
  void setCursor(int x, int y) override;
  void print(const char* str) override;
  void printWordWrap(const char* str, int max_width) override;
  void translateUTF8ToBlocks(char* dest, const char* src, size_t dest_size) override;
  void fillRect(int x, int y, int w, int h) override;
  void drawRect(int x, int y, int w, int h) override;
  void drawXbm(int x, int y, const uint8_t* bits, int w, int h) override;
  uint16_t getTextWidth(const char* str) override;
  void endFrame() override;
};

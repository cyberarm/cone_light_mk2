#pragma once

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "cone_light.h"

// Forward declaration...
class ConeLight;

class ConeLightDisplay
{
private:
  Adafruit_SSD1306 *m_display = new Adafruit_SSD1306(128, 64, &Wire, -1);
  bool m_display_inited = false, m_bootscreen_shown = false;
  unsigned long m_last_milliseconds = millis();
  ConeLight* m_cone_light = nullptr;

public:
  ConeLightDisplay(ConeLight* cone_light);
  ~ConeLightDisplay();
  void update();
  void display_bootscreen();
  void display_menu();
  void draw_up_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_down_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_left_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_right_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_select_icon(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
};

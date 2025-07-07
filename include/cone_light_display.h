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
  uint32_t m_last_milliseconds = millis();
  ConeLight *m_cone_light = nullptr;
  uint8_t m_widget_bar_height = 14;
  uint8_t m_last_battery_meter_width = 16;
  uint8_t m_battery_meter_width = 16;
  bool m_last_lid_state = false;
  bool m_last_sync_state = false;
  bool m_widget_bar_needs_redraw = true;

public:
  ConeLightDisplay(ConeLight *cone_light);
  ~ConeLightDisplay();
  Adafruit_SSD1306 *oled() { return m_display; };
  uint8_t widget_bar_height() {return m_widget_bar_height; };
  void update();
  void display_bootscreen();
  void display_menu();
  bool widget_bar_needs_redraw();
  void draw_widget_bar();
  void draw_up_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_down_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_left_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_right_arrow(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
  void draw_right_arrow(uint16_t x, uint16_t y, uint16_t color, bool tail);
  void draw_select_icon(uint16_t x, uint16_t y, uint16_t color = SSD1306_WHITE);
};

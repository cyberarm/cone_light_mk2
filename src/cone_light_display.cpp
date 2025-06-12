#include "cone_light_lighting.h"

ConeLightDisplay::ConeLightDisplay(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  if (m_display->begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    m_display_inited = true;

    m_display->invertDisplay(false);
    m_display->setTextSize(1);
    m_display->setTextColor(SSD1306_WHITE);
    m_display->cp437(true);
  }

  if (m_display_inited)
  {
    Serial.println("    Display initialized successfully.");
  }
  else
  {
    Serial.println("    ERROR: Failed to init display.");
  }
}

ConeLightDisplay::~ConeLightDisplay()
{
  delete m_display;
}

void ConeLightDisplay::update()
{
}

void ConeLightDisplay::draw_widgets()
{
  m_display->drawRect(0, 0, 128, m_widget_bar_height, SSD1306_WHITE);

  m_display->setCursor(4, 4);
  // Display Node Name
  m_display->print(m_cone_light->node_name());
  m_display->drawFastVLine(42, 0, m_widget_bar_height, SSD1306_WHITE);

  // Display LID indicator
  m_display->print(m_cone_light->input_handler()->lid_open() ? " LID" : "    ");

  // Display master clock sync status
  m_display->print(true ? " SYNC" : "     ");

  // Display battery voltage meter
  m_display->drawRect(103, 3, 20, m_widget_bar_height - 6, SSD1306_WHITE);
  m_display->drawRect(123, m_widget_bar_height / 2 - 2, 2, 4, SSD1306_WHITE);
  // --- battery bar
  float voltage_ratio = m_cone_light->voltage()->voltage_percentage() / 100.0;
  m_display->fillRect(105, 5, (int16_t)(16 * voltage_ratio), m_widget_bar_height - 10, SSD1306_WHITE);
}

void ConeLightDisplay::draw_up_arrow(uint16_t x, uint16_t y, uint16_t color)
{
  m_display->drawFastVLine(x + 3, y + 3, 6, color);
  m_display->fillTriangle(x, y + 3, x + 3, y, x + 6, y + 3, color);
}

void ConeLightDisplay::draw_down_arrow(uint16_t x, uint16_t y, uint16_t color)
{
  m_display->drawFastVLine(x + 3, y, 6, color);
  m_display->fillTriangle(x, y + 6, x + 3, y + 6 + 3, x + 6, y + 6, color);
}

void ConeLightDisplay::draw_left_arrow(uint16_t x, uint16_t y, uint16_t color)
{
  m_display->drawFastHLine(x + 3, y, 6, color);
  m_display->fillTriangle(x + 3, y - 3, x, y, x + 3, y + 3, color);
}

void ConeLightDisplay::draw_right_arrow(uint16_t x, uint16_t y, uint16_t color)
{
  m_display->drawFastHLine(x, y, 6, color);
  m_display->fillTriangle(x + 6, y - 3, x + 6 + 3, y, x + 6, y + 3, color);
}

void ConeLightDisplay::draw_select_icon(uint16_t x, uint16_t y, uint16_t color)
{
  m_display->drawCircle(x, y, 5, color);
}

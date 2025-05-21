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

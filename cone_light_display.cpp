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
    display_bootscreen();
    // display_menu();
  }
  else
  {
    Serial.println("ERROR: Failed to init display.");
  }
}

ConeLightDisplay::~ConeLightDisplay()
{
  delete m_display;
}

void ConeLightDisplay::update()
{
  if (!m_bootscreen_shown && millis() - m_last_milliseconds >= 3'000)
  {
    m_bootscreen_shown = true;
    m_last_milliseconds = millis();

    display_menu();
  }
}

void ConeLightDisplay::display_bootscreen()
{
  m_display->clearDisplay();

  // Cone
  m_display->fillTriangle(12, 43, 20, 4, 43, 43, SSD1306_WHITE);
  m_display->fillTriangle(20, 4, 35, 4, 43, 43, SSD1306_WHITE);
  m_display->fillRect(4, 44, 48, 4, SSD1306_WHITE);

  // Cross
  m_display->fillRect(20, 15 + 4, 16, 4, SSD1306_BLACK);
  m_display->fillRect(26, 8 + 4, 4, 36 - 8, SSD1306_BLACK);

  // Labels
  m_display->setCursor(11, 54);
  m_display->print(m_cone_light->node_name());
  m_display->printf("  v%s", CONE_LIGHT_FIRMWARE_VERSION_NAME);

  m_display->setCursor(56, 12);
  m_display->print("Cone Light");
  m_display->setCursor(68, 24);
  m_display->print("mk. II");

  m_display->display();
}

void ConeLightDisplay::display_menu()
{
  m_display->clearDisplay();

  draw_up_arrow(7, 6);
  m_display->drawFastHLine(0, 21, 20, SSD1306_WHITE);
  draw_select_icon(7 + 3, 32);
  m_display->drawFastHLine(0, 42, 20, SSD1306_WHITE);
  draw_down_arrow(7, 64 - (9 + 8));

  m_display->drawFastVLine(20, 0, 64, SSD1306_WHITE);

  // Border
  m_display->drawRect(0, 0, 128, 64, SSD1306_WHITE);

  // Labels
  m_display->setCursor(32, 32 - 3);
  m_display->print("SMART CONTROL");

  m_display->setCursor(42, 4);
  m_display->print("manual control");
  m_display->setCursor(42, 64 - (7 + 4));
  m_display->print("airplane mode");

  m_display->display();
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

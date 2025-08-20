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
  // Check wether widget bar needs to be redrawn
  m_widget_bar_needs_redraw = m_last_lid_state != m_cone_light->input_handler()->lid_open() ||
                              m_last_sync_state != m_cone_light->network_time()->is_clock_synced() ||
                              m_last_battery_meter_width != m_battery_meter_width;

  m_last_battery_meter_width = m_battery_meter_width;

  float voltage_ratio = m_cone_light->voltage()->voltage_percentage() / 100.0;
  m_battery_meter_width = (int8_t)(16 * voltage_ratio);

  m_last_lid_state = m_cone_light->input_handler()->lid_open();
  m_last_sync_state = m_cone_light->network_time()->is_clock_synced();
}

bool ConeLightDisplay::widget_bar_needs_redraw()
{
  return m_widget_bar_needs_redraw;
}

void ConeLightDisplay::draw_widget_bar()
{
  // Clear widget bar area for redrawing
  m_display->fillRect(0, 0, 128, m_widget_bar_height, SSD1306_BLACK);

  m_display->drawRect(0, 0, 128, m_widget_bar_height, SSD1306_WHITE);

  m_display->setCursor(4, 4);
  // Display Node Name
  m_display->print(m_cone_light->node_name());
  m_display->drawFastVLine(42, 0, m_widget_bar_height, SSD1306_WHITE);

  // Display Node ID and Node Group ID
  m_display->printf(" %d:%d", m_cone_light->node_id(), m_cone_light->node_group_id());
  m_display->drawFastVLine(42 + 24, 0, m_widget_bar_height, SSD1306_WHITE);

  // Display LID indicator
  m_display->print(m_cone_light->input_handler()->lid_open() ? " LID" : "    ");
  m_display->drawFastVLine(42 + 24 + 24, 0, m_widget_bar_height, SSD1306_WHITE);

  // // Display master clock sync status
  // m_display->print((m_cone_light->network_time()->is_clock_synced() && std::abs(m_cone_light->network_time()->offset()) < 25) ? " SYNC" : "     ");

  // Display battery voltage meter
  m_display->drawRect(103, 3, 20, m_widget_bar_height - 6, SSD1306_WHITE);
  m_display->drawRect(123, m_widget_bar_height / 2 - 2, 2, 4, SSD1306_WHITE);
  // --- battery bar
  m_display->fillRect(105, 5, m_battery_meter_width, m_widget_bar_height - 10, SSD1306_WHITE);
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

void ConeLightDisplay::draw_right_arrow(uint16_t x, uint16_t y, uint16_t color, bool tail)
{
  if (tail)
    m_display->drawFastHLine(x, y, 6, color);
  m_display->fillTriangle(x + 6, y - 3, x + 6 + 3, y, x + 6, y + 3, color);
}

void ConeLightDisplay::draw_right_arrow(uint16_t x, uint16_t y, uint16_t color)
{
  ConeLightDisplay::draw_right_arrow(x, y, color, true);
}

void ConeLightDisplay::draw_select_icon(uint16_t x, uint16_t y, uint16_t color)
{
  m_display->drawCircle(x, y, 5, color);
}

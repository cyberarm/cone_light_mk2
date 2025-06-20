#include "cone_light_application.h"

//--- ||| ---//
ConeLightApplication::ConeLightApplication(ConeLight *cone_light)
{
}

ConeLightDisplay *ConeLightApplication::display() { return m_cone_light->display(); };

Adafruit_SSD1306 *ConeLightApplication::oled() { return display()->oled(); };

//--- BOOT SCREEN APP ---//
ConeLight_App_BootScreen::ConeLight_App_BootScreen(ConeLight *cone_light) : ConeLightApplication(cone_light)
{
  m_cone_light = cone_light;
  m_app_name = "Boot Screen";
  m_fullscreen = true;

  // Set default color based on node group
  // GOLD for group 1, TEAL for group 2, and PURPLE for not configured (group 255).
  if (m_cone_light->node_group_id() == CONE_LIGHT_NODE_GROUP_0)
  {
    m_group_color = COLOR_GOLD;
  }
  else if (m_cone_light->node_group_id() == CONE_LIGHT_NODE_GROUP_1)
  {
    m_group_color = COLOR_TEAL;
  }
  else
  {
    m_group_color = COLOR_PURPLE;
  }

  m_cone_light->lighting()->set_color(m_boot_color);
};

void ConeLight_App_BootScreen::draw()
{
  // Cone
  oled()->fillTriangle(12, 43, 20, 4, 43, 43, SSD1306_WHITE);
  oled()->fillTriangle(20, 4, 35, 4, 43, 43, SSD1306_WHITE);
  oled()->fillRect(4, 44, 48, 4, SSD1306_WHITE);

  // Cross
  oled()->fillRect(20, 15 + 4, 16, 4, SSD1306_BLACK);
  oled()->fillRect(26, 8 + 4, 4, 36 - 8, SSD1306_BLACK);

  // Labels
  oled()->setCursor(11, 54);
  oled()->print(m_cone_light->node_name());
  oled()->printf("  v%s", CONE_LIGHT_FIRMWARE_VERSION_NAME);

  oled()->setCursor(56, 12);
  oled()->print("Cone Light");
  oled()->setCursor(68, 24);
  oled()->print("mk. II");
}

void ConeLight_App_BootScreen::update()
{
  if (millis() - m_last_milliseconds < 667)
    m_cone_light->lighting()->set_brightness(2);
  else if (millis() - m_last_milliseconds < (667 + 32) * 2)
    m_cone_light->lighting()->set_brightness(4);
  else if (millis() - m_last_milliseconds < (667 + 32 + 32) * 3)
    m_cone_light->lighting()->set_brightness(6);
  else if (millis() - m_last_milliseconds < (667 + 32 + 32 + 32) * 4)
    m_cone_light->lighting()->set_brightness(8);
  else
  {
    m_cone_light->lighting()->set_color(m_group_color);
    m_cone_light->boot_complete();
  }

  m_needs_redraw = false;
}

void ConeLight_App_BootScreen::button_down(ConeLightButton btn)
{
  if (millis() >= 3'000 && btn == BUTTON_A)
  {
    m_cone_light->lighting()->set_color(210, 21, 2);
    m_cone_light->lighting()->set_brightness(255);
  }

  // Boot complete
  if (millis() >= 3'000 && btn == BUTTON_B)
    m_cone_light->boot_complete();

  if (millis() >= 3'000 && btn == BUTTON_C)
    m_cone_light->speaker()->play_song(0);
}

//--- MAIN MENU APP ---//
ConeLight_App_MainMenu::ConeLight_App_MainMenu(ConeLight *cone_light) : ConeLightApplication(cone_light)
{
  m_cone_light = cone_light;
  m_app_name = "Main Menu";

  m_max_app_index = 0;
}

void ConeLight_App_MainMenu::draw()
{
  // Draw UP arrow
  display()->draw_up_arrow(7, 2 + display()->widget_bar_height());
  oled()->drawFastHLine(0, 28, 20, SSD1306_WHITE);
  // Draw SELECT icon
  display()->draw_select_icon(7 + 3, 32 + (display()->widget_bar_height() / 2) - 1);
  oled()->drawFastHLine(0, 48, 20, SSD1306_WHITE);
  // Draw DOWN arrow
  display()->draw_down_arrow(7, 64 - (9 + 4));

  // Draw vertical line to box off the arrows and select icon
  oled()->drawFastVLine(20, display()->widget_bar_height(), 64, SSD1306_WHITE);

  // Border
  oled()->drawRect(0, (display()->widget_bar_height() - 1), 128, 64 - (display()->widget_bar_height() - 1), SSD1306_WHITE);

  // Labels
  uint8_t previous_app_index = m_app_index - 1;
  if (previous_app_index < 2)
    previous_app_index = m_max_app_index;
  if (previous_app_index > m_max_app_index)
    previous_app_index = 2;

  uint8_t next_app_index = m_app_index + 1;
  if (next_app_index < 2)
    next_app_index = m_max_app_index;
  if (next_app_index > m_max_app_index)
    next_app_index = 2;

  oled()->setCursor(42, 3 + display()->widget_bar_height());
  oled()->print(m_cone_light->applications()[next_app_index]->name());

  oled()->setCursor(32, 32 + ((display()->widget_bar_height() / 2) - 1) - 3);
  oled()->print(m_cone_light->applications()[m_app_index]->name());
  // oled()->printf("%.3fv (%3.1f%%)", m_cone_light->voltage()->voltage(), m_cone_light->voltage()->voltage_percentage());

  oled()->setCursor(42, 64 - ((display()->widget_bar_height() / 2) + 4));
  oled()->print(m_cone_light->applications()[previous_app_index]->name());
}

void ConeLight_App_MainMenu::update()
{
  if (m_max_app_index == 0)
  {
    m_max_app_index = m_cone_light->applications().size() - 1;
    m_needs_redraw = true;
  }
  else
  {
    m_needs_redraw = false;
  }
}

void ConeLight_App_MainMenu::button_down(ConeLightButton btn)
{
  switch (btn)
  {
  case UP_BUTTON:
    m_app_index++;
    if (m_app_index < 2)
      m_app_index = m_max_app_index;
    if (m_app_index > m_max_app_index)
      m_app_index = 2;

    Serial.printf("%d\n", m_app_index);

    m_needs_redraw = true;
    break;
  case SELECT_BUTTON:
    m_cone_light->current_app()->blur();
    Serial.printf("SELECTED: %s\n", m_cone_light->applications()[m_app_index]->name().c_str());
    m_cone_light->set_current_app(m_cone_light->applications()[m_app_index]);
    m_cone_light->current_app()->focus();
    break;
  case DOWN_BUTTON:
    m_app_index--;
    if (m_app_index < 2)
      m_app_index = m_max_app_index;
    if (m_app_index > m_max_app_index)
      m_app_index = 2;

    Serial.printf("%d\n", m_app_index);

    m_needs_redraw = true;
    break;

  default:
    break;
  }
}

//--- MANUAL CONTROL APP ---//
void ConeLight_App_ManualControl::draw() {}

//--- SMART CONTROL APP ---//
void ConeLight_App_SmartControl::draw() {}

//--- NODE CONFIGURATION APP ---//
void ConeLight_App_NodeInfo::draw()
{
  // FIXME: This is duplicated from the BootScreen App, this should probably be shared :)

  // Cone
  oled()->fillTriangle(12, 43, 20, 4, 43, 43, SSD1306_WHITE);
  oled()->fillTriangle(20, 4, 35, 4, 43, 43, SSD1306_WHITE);
  oled()->fillRect(4, 44, 48, 4, SSD1306_WHITE);

  // Cross
  oled()->fillRect(20, 15 + 4, 16, 4, SSD1306_BLACK);
  oled()->fillRect(26, 8 + 4, 4, 36 - 8, SSD1306_BLACK);

  // Labels
  oled()->setCursor(11, 54);
  oled()->print(m_cone_light->node_name());
  oled()->printf("  v%s", CONE_LIGHT_FIRMWARE_VERSION_NAME);

  oled()->setCursor(56, 4);
  oled()->print("Cone Light");
  oled()->setCursor(68, 14);
  oled()->print("mk. II");
  oled()->setCursor(78, 30);
  oled()->printf("%d:%d", m_cone_light->node_id(), m_cone_light->node_group_id());
  oled()->setCursor(64, 41);
  oled()->printf("%s", (m_cone_light->node_group_id() == 0) ? CONE_LIGHT_NODE_GROUP_0_NAME : (m_cone_light->node_group_id() == 1) ? CONE_LIGHT_NODE_GROUP_1_NAME
                                                                                                                                  : CONE_LIGHT_NODE_GROUP_255_NAME);
}

void ConeLight_App_NodeInfo::button_down(ConeLightButton btn)
{
  m_cone_light->set_current_app_main_menu();
}

//--- BATTERY INFORMATION APP ---//
void ConeLight_App_BatteryInfo::draw()
{
  uint8_t half_widget_height = display()->widget_bar_height() / 2;
  uint8_t battery_width = 48;
  uint8_t battery_height = 20;
  uint8_t battery_bar_width = battery_width - 6;
  uint8_t battery_bar_height = battery_height - 6;

  battery_bar_width = battery_bar_width * std::clamp(m_voltage_ratio, 0.0f, 1.0f);

  // Display battery voltage meter
  oled()->drawRect(128 / 2 - battery_width / 2, 64 / 2 - battery_height / 2 + half_widget_height, battery_width, battery_height, SSD1306_WHITE);
  oled()->fillRect(128 / 2 + battery_width / 2, 64 / 2 - 5 + half_widget_height, 3, 10, SSD1306_WHITE);
  // --- battery bar
  oled()->fillRect(128 / 2 - battery_width / 2 + 3, 64 / 2 - battery_height / 2 + 3 + half_widget_height, battery_bar_width, battery_bar_height, SSD1306_WHITE);

  // Voltage percentage
  oled()->setCursor(128 / 2 - 20, 64 / 2 - battery_height / 2 - 4);
  oled()->print(std::format("{:6.2f}%", m_voltage_percentage).c_str());

  // Minimum voltage
  oled()->setCursor(6, 64 / 2 - 4 + half_widget_height);
  oled()->printf("%.2fv", VOLTAGE_MIN);

  // Maximum voltage
  oled()->setCursor(96, 64 / 2 - 4 + half_widget_height);
  oled()->printf("%.2fv", VOLTAGE_MAX);

  // Current voltage
  oled()->setCursor(128 / 2 - 20, 64 / 2 - battery_height / 2 + half_widget_height + battery_height + 4);
  oled()->printf("%.4fv", m_cone_light->voltage()->voltage());
}

void ConeLight_App_BatteryInfo::update()
{
  m_voltage_percentage = m_cone_light->voltage()->voltage_percentage();
  m_voltage_ratio = m_voltage_percentage / 100.0;

  m_needs_redraw = m_last_battery_voltage_percentage != m_voltage_percentage;

  m_last_battery_voltage_percentage = m_voltage_percentage;
}

void ConeLight_App_BatteryInfo::button_down(ConeLightButton btn)
{
  m_cone_light->set_current_app_main_menu();
}

//--- DEBUG: ESPNOW SENDER APP ---//
void ConeLight_App_Debug_ESPNow_Sender::draw()
{
  oled()->setCursor(24, 18);
  oled()->printf("SENT: %d", m_packet_id);
}

void ConeLight_App_Debug_ESPNow_Sender::update()
{
  if (millis() - m_last_transmit_ms < 500)
    return;

  cone_light_network_packet_t packet;
  packet.packet_id = m_packet_id++;
  packet.node_id = m_cone_light->node_id();

  m_cone_light->networking()->broadcast_packet(packet);

  m_last_transmit_ms = millis();
}

void ConeLight_App_Debug_ESPNow_Sender::button_down(ConeLightButton btn)
{
  blur();
  m_cone_light->set_current_app_main_menu();
}

//--- DEBUG: ESPNOW RECEIVER APP ---//
void ConeLight_App_Debug_ESPNow_Receiver::draw()
{
  oled()->setCursor(24, 18);
  oled()->printf("RECV: %d", m_last_packet_id);
  oled()->setCursor(24, 26);
  oled()->printf("LOST: %d", m_total_packets_lost);
  oled()->setCursor(24, 34);
  oled()->printf("LAST: +%d", m_packets_lost);
}

void ConeLight_App_Debug_ESPNow_Receiver::update()
{
}

void ConeLight_App_Debug_ESPNow_Receiver::button_down(ConeLightButton btn)
{
  blur();
  m_cone_light->set_current_app_main_menu();
}

void ConeLight_App_Debug_ESPNow_Receiver::espnow_recv(cone_light_network_packet_t packet)
{
  unsigned int packets_lost = packet.packet_id - m_last_packet_id;
  // If 1 packet is 'lost' then we haven't lost any (2 - 1 = 1) [PKT_ID - LAST_PKT_ID = 1]
  if (packets_lost == 1)
    packets_lost = 0;

  m_total_packets_lost += packets_lost;
  m_packets_lost = packets_lost;

  // Serial.printf("PKT LOSS: %d (%d)\n", m_packets_lost, packets_lost);

  m_last_packet_id = packet.packet_id;
}

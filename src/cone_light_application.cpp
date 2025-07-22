#include "cone_light_application.h"
#include "cone_light_songs.h"

//--- ||| ---//
ConeLightApplication::ConeLightApplication(ConeLight *cone_light)
{
}

ConeLightDisplay *ConeLightApplication::display() { return m_cone_light->display(); };

Adafruit_SSD1306 *ConeLightApplication::oled() { return display()->oled(); };

///////////////////////////
//--- BOOT SCREEN APP ---//
///////////////////////////
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
  m_cone_light->lighting()->set_static_color(m_group_color);
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
    m_cone_light->lighting()->set_brightness(LED_DEFAULT_BRIGHTNESS / 4);
  else if (millis() - m_last_milliseconds < (667 + 32) * 2)
    m_cone_light->lighting()->set_brightness(LED_DEFAULT_BRIGHTNESS / 4 * 2);
  else if (millis() - m_last_milliseconds < (667 + 32 + 32) * 3)
    m_cone_light->lighting()->set_brightness(LED_DEFAULT_BRIGHTNESS / 4 * 3);
  else if (millis() - m_last_milliseconds < (667 + 32 + 32 + 32) * 4)
    m_cone_light->lighting()->set_brightness(LED_DEFAULT_BRIGHTNESS);
  else
  {
    m_cone_light->lighting()->set_color(m_group_color);
    m_cone_light->boot_complete();
  }

  m_needs_redraw = false;
}

bool ConeLight_App_BootScreen::button_down(ConeLightButton btn)
{
  if (millis() >= 3'000 && btn == BUTTON_A)
  {
    m_cone_light->lighting()->set_color(210, 21, 2);
    m_cone_light->lighting()->set_brightness(255);
    return true;
  }

  // Boot complete
  if (millis() >= 3'000 && btn == BUTTON_B)
    m_cone_light->boot_complete();
  return true;

  if (millis() >= 3'000 && btn == BUTTON_C)
    m_cone_light->speaker()->play_song(0);
  return true;

  return false;
}

/////////////////////////
//--- MAIN MENU APP ---//
/////////////////////////
ConeLight_App_MainMenu::ConeLight_App_MainMenu(ConeLight *cone_light) : ConeLightApplication(cone_light)
{
  m_cone_light = cone_light;
  m_app_name = "Main Menu";

  m_max_app_index = 0;
}

void ConeLight_App_MainMenu::draw()
{
  oled()->setTextWrap(false);

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

  oled()->setTextWrap(true);
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

bool ConeLight_App_MainMenu::button_down(ConeLightButton btn)
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
    return false;
    break;
  }

  return true;
}

////////////////////////////////////////
//--- LED CONTROL <BASE CLASS> APP ---//
////////////////////////////////////////
void ConeLight_App_LEDControl::draw()
{
  // Draw top bar
  oled()->drawRect(0, 0, 128, display()->widget_bar_height(), SSD1306_WHITE);
  // Draw back label
  oled()->setCursor(4, 4);
  oled()->print(m_labels[m_index].c_str());
  // Draw hex colour label
  oled()->setCursor(72, 4);
  oled()->print(hex_color(m_values[1], m_values[2], m_values[3], m_values[4]).c_str());

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

  for (size_t i = 1; i <= m_max_index; i++)
  {
    uint16_t width = 64;
    uint16_t height = 8;
    uint16_t padding = 4;
    uint16_t x = 34;
    uint16_t y = display()->widget_bar_height() + 2 + ((height + padding) * (i - 1));

    float ratio = 0.0f;
    ratio = std::clamp(0.0f, 1.0f, (float)m_values[i] / 255.0f);

    // label
    oled()->setCursor(25, y + 1);
    oled()->print(m_labels[i].c_str()[0]);
    // box
    if (m_selected && m_index == i)
      oled()->drawRect(x, y, width, height, WHITE);
    // bar
    oled()->fillRect(x + 2, y + 2, (width - 4) * ratio, height - 4, WHITE);
    // percentage
    oled()->setCursor(x + width + 2, y + 1);
    oled()->print(std::format("{:3}%", uint8_t(ratio * 100.0f)).c_str());

    if (!m_selected && m_index == i)
    {
      display()->draw_right_arrow(30, y + 4, BLACK, false);
      display()->draw_right_arrow(28, y + 4, WHITE, false);
    }
  }
}

void ConeLight_App_LEDControl::update()
{
  if (m_held_direction != ConeLightDirection::NONE && millis() - m_last_held_increment_ms >= 50)
  {
    m_last_held_increment_ms = millis();
    m_values[m_index] += m_held_direction;
    apply_color();
  }

  // Continually reset last held when not held
  if (m_held_direction == ConeLightDirection::NONE)
    m_last_held_increment_ms = millis();

  m_needs_redraw = m_held_direction != ConeLightDirection::NONE;
}

bool ConeLight_App_LEDControl::button_down(ConeLightButton btn)
{
  m_needs_redraw = true;

  switch (btn)
  {
  case UP_BUTTON:
    if (m_selected)
    {
      m_values[m_index]++;
      apply_color();
    }
    else
    {
      m_index--;
      if (m_index < 0)
        m_index = m_max_index;
      // Handle integer underflow
      if (m_index > m_max_index)
        m_index = m_max_index;
    }
    return true;

    break;

  case SELECT_BUTTON:
    if (m_selected)
    {
      m_selected = false;
      return true;
    }
    else
    {
      if (m_index == 0)
      {
        m_cone_light->set_current_app_main_menu();
        m_selected = false;
      }
      else
      {
        m_selected = true;
      }
      return true;
    }
    break;

  case DOWN_BUTTON:
    if (m_selected)
    {
      m_values[m_index]--;
      apply_color();
    }
    else
    {
      m_index++;
      if (m_index > m_max_index)
        m_index = 0;
    }
    return true;

    break;

  default:
    return false;
    break;
  }

  return false;
}

bool ConeLight_App_LEDControl::button_held(ConeLightButton btn)
{
  if (!m_selected)
    return false;

  m_needs_redraw = true;

  switch (btn)
  {
  case UP_BUTTON:
    m_held_direction = ConeLightDirection::UP;
    return true;
    break;
  case DOWN_BUTTON:
    m_held_direction = ConeLightDirection::DOWN;
    return true;
    break;

  default:
    return false;
    break;
  }

  return false;
}

bool ConeLight_App_LEDControl::button_up(ConeLightButton btn)
{
  m_needs_redraw = true;

  if (m_held_direction != ConeLightDirection::NONE)
  {
    m_held_direction = ConeLightDirection::NONE;
    return true;
  }

  return false;
}

void ConeLight_App_LEDControl::focus()
{
  m_needs_redraw = true;

  if (!m_has_been_focused_once)
  {
    m_has_been_focused_once = true;

    CRGB color = m_cone_light->lighting()->get_color();
    m_values[1] = color.red;
    m_values[2] = color.green;
    m_values[3] = color.blue;

    m_values[4] = m_cone_light->lighting()->get_brightness();
  }
}

void ConeLight_App_LEDControl::apply_color()
{
  CRGB color = CRGB(m_values[1], m_values[2], m_values[3]);

  m_cone_light->lighting()->set_color(color);
  m_cone_light->lighting()->set_static_color(color);
  m_cone_light->lighting()->set_brightness(m_values[4]);

  if (m_cone_light_led_control_mode == GROUP || m_cone_light_led_control_mode == ALL)
  {
    cone_light_network_packet_t packet = {};
    uint32_t packed_color = uint32_t{m_values[4]} << 24 |
                            (uint32_t{color.red} << 16) |
                            (uint32_t{color.green} << 8) |
                            (uint32_t{color.blue});

    packet.command_id = m_cone_light->networking()->next_command_id();
    packet.command_type = (m_cone_light_led_control_mode == GROUP) ? ConeLightNetworkCommand::SET_GROUP_COLOR : ConeLightNetworkCommand::SET_COLOR;
    packet.command_parameters = packed_color;
    packet.command_parameters_extra = m_cone_light->node_group_id();

    m_cone_light->networking()->broadcast_packet(packet);
  }
}

/////////////////////
//--- SONGS APP ---//
/////////////////////
void ConeLight_App_Songs::draw()
{
  oled()->setTextWrap(false);

  oled()->setCursor(2, 3);
  oled()->print("Hold UP for Main Menu");

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
  uint8_t previous_song_index = m_song_index - 1;
  if (previous_song_index > m_max_song_index)
    previous_song_index = m_max_song_index;

  uint8_t next_song_index = m_song_index + 1;
  if (next_song_index > m_max_song_index)
    next_song_index = 0;

  oled()->setCursor(42, 3 + display()->widget_bar_height());
  oled()->print(cone_light_songs[previous_song_index].name());

  oled()->setCursor(32, 32 + ((display()->widget_bar_height() / 2) - 1) - 3);
  oled()->print(cone_light_songs[m_song_index].name());

  oled()->setCursor(42, 64 - ((display()->widget_bar_height() / 2) + 4));
  oled()->print(cone_light_songs[next_song_index].name());

  oled()->setTextWrap(true);
}

void ConeLight_App_Songs::update()
{
  if (m_max_song_index == 0)
  {
    m_max_song_index = cone_light_songs.size() - 1;
    m_needs_redraw = true;
  }
  else
  {
    m_needs_redraw = false;
  }
}

bool ConeLight_App_Songs::button_down(ConeLightButton btn)
{
  switch (btn)
  {
  case UP_BUTTON:
    m_song_index--;
    if (m_song_index > m_max_song_index)
      m_song_index = m_max_song_index;

    Serial.printf("%d\n", m_song_index);

    m_needs_redraw = true;
    break;
  case SELECT_BUTTON:
  {
    cone_light_network_packet_t packet = {};
    packet.command_id = m_cone_light->networking()->next_command_id();
    packet.command_type = ConeLightNetworkCommand::PLAY_SONG;
    packet.command_parameters = m_song_index;
    packet.command_parameters_extra = 255;

    m_cone_light->networking()->broadcast_packet(packet);
    // inject packet into the commanding node to make it handle the command itself too
    m_cone_light->espnow_event(packet);
    break;
  }
  case DOWN_BUTTON:
    m_song_index++;
    if (m_song_index > m_max_song_index)
      m_song_index = 0;

    Serial.printf("%d\n", m_song_index);

    m_needs_redraw = true;
    break;

  default:
    return false;
    break;
  }

  return true;
}

bool ConeLight_App_Songs::button_held(ConeLightButton btn)
{
  switch (btn)
  {
  case UP_BUTTON:
    m_cone_light->set_current_app_main_menu();

    return true;
    break;

  default:
    return false;
    break;
  }

  return false;
}

//////////////////////////////////
//--- NODE CONFIGURATION APP ---//
//////////////////////////////////
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

bool ConeLight_App_NodeInfo::button_down(ConeLightButton btn)
{
  m_cone_light->set_current_app_main_menu();

  return true;
}

///////////////////////////////////
//--- BATTERY INFORMATION APP ---//
///////////////////////////////////
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

bool ConeLight_App_BatteryInfo::button_down(ConeLightButton btn)
{
  m_cone_light->set_current_app_main_menu();

  return true;
}

////////////////////////////////////
//--- DEBUG: ESPNOW SENDER APP ---//
////////////////////////////////////
void ConeLight_App_Debug_ESPNow_Sender::draw()
{
  oled()->setCursor(24, 18);
  oled()->printf("SENT: %d", m_packet_id);

  m_needs_redraw = false;
}

void ConeLight_App_Debug_ESPNow_Sender::update()
{
  if (millis() - m_last_transmit_ms < 500)
    return;

  cone_light_network_packet_t packet;
  packet.command_id = m_cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::NOT_A_COMMAND;
  packet.command_parameters = m_packet_id++;

  m_cone_light->networking()->broadcast_packet(packet);

  m_last_transmit_ms = millis();

  m_needs_redraw = true;
}

bool ConeLight_App_Debug_ESPNow_Sender::button_down(ConeLightButton btn)
{
  blur();
  m_cone_light->set_current_app_main_menu();

  return true;
}

//////////////////////////////////////
//--- DEBUG: ESPNOW RECEIVER APP ---//
//////////////////////////////////////
void ConeLight_App_Debug_ESPNow_Receiver::draw()
{
  oled()->setTextWrap(false);

  oled()->setCursor(24, 18);
  oled()->printf("RECV: %d", m_last_packet_id);
  oled()->setCursor(24, 26);
  oled()->printf("LOST: %d", m_total_packets_lost);
  oled()->setCursor(24, 34);
  oled()->printf("LAST: +%d", m_packets_lost);
  oled()->setCursor(24, 42);
  oled()->printf("FROM: %d:%s:%d", m_last_sender_id, m_last_sender_name, m_last_sender_group_id);
  oled()->setCursor(24, 50);
  oled()->printf("RSSI: %d", m_cone_light->networking()->node_rssi(m_last_sender_id));

  oled()->setTextWrap(true);

  m_needs_redraw = false;
}

void ConeLight_App_Debug_ESPNow_Receiver::update()
{
}

bool ConeLight_App_Debug_ESPNow_Receiver::button_down(ConeLightButton btn)
{
  blur();
  m_cone_light->set_current_app_main_menu();

  return true;
}

void ConeLight_App_Debug_ESPNow_Receiver::espnow_recv(cone_light_network_packet_t packet)
{
  uint16_t packets_lost = packet.packet_id - m_last_packet_id;
  // If 1 packet is 'lost' then we haven't lost any (2 - 1 = 1) [PKT_ID - LAST_PKT_ID = 1]
  if (packets_lost == 1)
    packets_lost = 0;

  m_total_packets_lost += packets_lost;
  m_packets_lost = packets_lost;

  // Serial.printf("PKT LOSS: %d (%d)\n", m_packets_lost, packets_lost);

  m_last_packet_id = packet.packet_id;
  m_last_sender_id = packet.node_id;
  strncpy(m_last_sender_name, packet.node_name, 7);
  m_last_sender_group_id = packet.node_group_id;

  m_needs_redraw = true;
}

void ConeLight_App_Debug_Network_Clock::draw()
{
  oled()->setCursor(4, 18);
  oled()->printf("GRAND MASTER: %s", m_cone_light->network_time()->is_grandmaster_clock() ? "true" : "false");
  oled()->setCursor(4, 26);
  oled()->printf("TIME SYNCED: %s", m_cone_light->network_time()->is_clock_synced() ? "true" : "false");
  oled()->setCursor(4, 34);
  oled()->printf("GRAND TIME: %u", m_cone_light->network_time()->time());
  oled()->setCursor(4, 42);
  oled()->printf("LOCAL TIME: %u", m_cone_light->network_time()->timestamp());
  oled()->setCursor(4, 50);
  oled()->printf("OFFSET MS: %d", m_cone_light->network_time()->offset());

  m_needs_redraw = false;
}

void ConeLight_App_Debug_Network_Clock::update()
{
  if (millis() - m_last_refresh_ms >= m_refresh_interval_ms)
  {
    m_last_refresh_ms = millis();
    m_needs_redraw = true;
  }
}

bool ConeLight_App_Debug_Network_Clock::button_down(ConeLightButton btn)
{
  blur();
  m_cone_light->set_current_app_main_menu();

  return true;
}

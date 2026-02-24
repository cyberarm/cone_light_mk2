#include "include/cone_light.h"

ConeLight::ConeLight()
{
  Serial.begin(115200);

  Serial.printf("Initializing %s v%s (internal v%d)\n", CONE_LIGHT_PRODUCT_NAME, CONE_LIGHT_FIRMWARE_VERSION_NAME, CONE_LIGHT_FIRMWARE_VERSION);
  Serial.println("  Configuring button inputs...");
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(BTN_C_PIN, INPUT_PULLUP);

  pinMode(LID_PIN, INPUT_PULLUP);

  // Load node specific data from Preferences
  Serial.println("  Loading node preferences data...");
  m_preferences.begin(CONE_LIGHT_PREFERENCES_ID);
  m_node_id = m_preferences.getUChar(CONE_LIGHT_PREFERENCES_NODE_ID, CONE_LIGHT_NODE_ID_UNSET);
  m_node_group = m_preferences.getUChar(CONE_LIGHT_PREFERENCES_NODE_GROUP, CONE_LIGHT_NODE_GROUP_ID_UNSET);
  m_node_name = m_preferences.getString(CONE_LIGHT_PREFERENCES_NODE_NAME, CONE_LIGHT_NODE_NAME_UNSET);
  m_node_grandmaster_clock = m_preferences.getBool(CONE_LIGHT_PREFERENCES_NODE_GRANDMASTER_CLOCK, CONE_LIGHT_NODE_GRAND_MASTER_CLOCK_UNSET_OR_FALSE);
  m_preferences.end();

  // Initialize subsystems...
  Serial.println("  Initializing subsystems...");
  m_command_handler = new ConeLightCommandHandler(this);
  m_input_handler = new ConeLightInputHandler(this);
  m_lighting = new ConeLightLighting(this);
  m_display = new ConeLightDisplay(this);
  m_speaker = new ConeLightSpeaker(this);
  m_voltage = new ConeLightVoltage(this);
  m_networking = new ConeLightNetworking(this);
  m_network_time = new ConeLightNetworkTime(this);

  m_applications.push_back(new ConeLight_App_BootScreen(this));
  m_applications.push_back(new ConeLight_App_MainMenu(this));
  m_applications.push_back(new ConeLight_App_NodeLEDControl(this));
  m_applications.push_back(new ConeLight_App_GroupNodeLEDControl(this));
  m_applications.push_back(new ConeLight_App_AllNodeLEDControl(this));
  m_applications.push_back(new ConeLight_App_Songs(this));
  m_applications.push_back(new ConeLight_App_NodeInfo(this));
  m_applications.push_back(new ConeLight_App_BatteryInfo(this));
  m_applications.push_back(new ConeLight_App_Debug_ESPNow_Sender(this));
  m_applications.push_back(new ConeLight_App_Debug_ESPNow_Receiver(this));
  m_applications.push_back(new ConeLight_App_Debug_Network_Clock(this));
  m_current_app = m_applications[0];

  m_last_input_change_ms = millis();
  m_screensaver = false;

  Serial.printf("Initialization of node %s (id: %d, group: %d) completed.\n", m_node_name.c_str(), m_node_id, m_node_group);
}

ConeLight::~ConeLight()
{
  delete m_network_time;
  delete m_networking;
  delete m_voltage;
  delete m_speaker;
  delete m_display;
  delete m_lighting;
  delete m_input_handler;
  delete m_command_handler;

  m_preferences.end();
  Serial.end();
}

void ConeLight::update()
{
  m_command_handler->update();
  m_input_handler->update();
  m_lighting->update();
  m_display->update();
  m_speaker->update();
  m_voltage->update();
  m_networking->update();
  m_network_time->update();

  update_screensaver();
  if (m_screensaver)
    return;

  // Check if the widget bar needs to be redrawn and the app doesn't need to be redrawn.
  if (!m_current_app || (m_current_app && !m_current_app->needs_redraw() && !m_current_app->fullscreen()))
  {
    if (m_display->widget_bar_needs_redraw())
    {
      m_display->draw_widget_bar();
      m_display->oled()->display();
    }
  }

  if (m_current_app)
  {
    if (m_current_app->needs_redraw())
    {
      m_display->oled()->clearDisplay();

      if (!m_current_app->fullscreen())
        m_display->draw_widget_bar();

      m_current_app->draw();

      m_display->oled()->display();
    }

    m_current_app->update();
  }
}

uint8_t ConeLight::node_id()
{
  return m_node_id;
}

uint8_t ConeLight::node_group_id()
{
  return m_node_group;
}

String ConeLight::node_name()
{
  return m_node_name;
}

bool ConeLight::node_grandmaster_clock()
{
  return m_node_grandmaster_clock;
}

bool ConeLight::reconfigure_node(uint8_t node_id, uint8_t node_group, String node_name, bool node_grandmaster_clock)
{
  m_preferences.begin(CONE_LIGHT_PREFERENCES_ID);
  m_preferences.putUChar(CONE_LIGHT_PREFERENCES_NODE_ID, node_id);
  m_preferences.putUChar(CONE_LIGHT_PREFERENCES_NODE_GROUP, node_group);
  m_preferences.putString(CONE_LIGHT_PREFERENCES_NODE_NAME, node_name);
  m_preferences.putBool(CONE_LIGHT_PREFERENCES_NODE_GRANDMASTER_CLOCK, node_grandmaster_clock);
  m_preferences.end();

  m_node_id = node_id;
  m_node_group = node_group;
  m_node_name = node_name;
  m_node_grandmaster_clock = node_grandmaster_clock;

  return true;
}

void ConeLight::boot_complete()
{
  set_current_app_main_menu();
}

void ConeLight::set_current_app_main_menu()
{
  if (m_current_app)
    m_current_app->blur();
  m_current_app = m_applications[1];
  m_current_app->focus();
}

void ConeLight::button_event(ConeLightButton btn, ConeLightEvent state)
{
  if (m_current_app && !m_screensaver) // 'Ignore' button press if screen is off (triggers wake up)
  {
    // TODO: Don't play button chirp if a song is playing.

    if (state == BUTTON_PRESSED)
      if (m_current_app->button_down(btn))
        if (!muted() && !speaker()->playing())
          speaker()->play_tone(BTN_PRESSED_SPEAKER_CHIRP_NOTE, BTN_SPEAKER_CHIRP_DURATION);
    if (state == BUTTON_HELD)
      if (m_current_app->button_held(btn))
        if (!muted() && !speaker()->playing())
          speaker()->play_tone(BTN_HELD_SPEAKER_CHIRP_NOTE, BTN_SPEAKER_CHIRP_DURATION);
    if (state == BUTTON_RELEASED)
      if (m_current_app->button_up(btn))
        if (!muted() && !speaker()->playing())
          speaker()->play_tone(BTN_RELEASED_SPEAKER_CHIRP_NOTE, BTN_SPEAKER_CHIRP_DURATION);
  }

  m_last_input_change_ms = millis();
}

void ConeLight::lid_event(ConeLightEvent state)
{
  if (m_current_app)
  {
    if (state == LID_CLOSED)
      m_current_app->lid_closed();
    if (state == LID_OPENED)
      m_current_app->lid_opened();
  }

  m_last_input_change_ms = millis();
}

void ConeLight::espnow_event(cone_light_network_packet_t packet)
{
  if (CONE_LIGHT_DEBUG)
  {
    Serial.printf(
        "RECV PACKET:\n"
        "    PROTOCOL ID: %s\n"
        "    FIRMWARE VERSION: %u\n"
        "    PACKET ID: %u\n"
        "    TIMESTAMP: %u\n"
        "    NODE ID: %u\n"
        "    NODE NAME: %s\n"
        "    NODE GROUP ID: %u\n"
        "    COMMAND ID: %u\n"
        "    COMMAND TYPE: %u\n"
        "    COMMAND PARAMETERS: %u\n"
        "    COMMAND PARAMETERS EXTRA: %u\n",
        packet.protocol_id,
        packet.firmware_version,
        packet.packet_id,
        packet.timestamp,
        packet.node_id,
        packet.node_name,
        packet.node_group_id,
        packet.command_id,
        packet.command_type,
        packet.command_parameters,
        packet.command_parameters_extra);
  }

  switch (packet.command_type)
  {
  case ConeLightNetworkCommand::NOT_A_COMMAND:
    if (m_current_app)
      m_current_app->espnow_recv(packet);
    break;

  case ConeLightNetworkCommand::CLOCK_GRANDMASTER_SYNC:
  case ConeLightNetworkCommand::CLOCK_NODE_DELAY_REQUEST:
  case ConeLightNetworkCommand::CLOCK_NODE_DELAY_RESPONSE:
    m_network_time->packet_handler(packet);
    break;

  case ConeLightNetworkCommand::SET_COLOR:
  case ConeLightNetworkCommand::SET_GROUP_COLOR:
    m_lighting->handle_packet(packet);
    break;

  case ConeLightNetworkCommand::PLAY_TONE:
  case ConeLightNetworkCommand::PLAY_SONG:
    m_speaker->handle_packet(packet);
    break;
  default:
    break;
  }
}

void ConeLight::update_screensaver()
{
  // Put display to 'sleep' after a bit of inactivity to preserve the oled display
  if (millis() - m_last_input_change_ms >= SCREENSAVER_TIMEOUT_MS)
  {
    if (!m_screensaver)
    {
      m_display->oled()->clearDisplay();
      m_display->oled()->display();

      m_screensaver = true;
    }

    if (m_current_app)
    {
      m_current_app->update();
    }

    return;
  }

  if (m_screensaver)
  {
    m_screensaver = false;

    // NOTE: May produce unexpected behavior. May be better to allow apps to be forced to be redrawn instead.
    if (m_current_app)
      m_current_app->focus();
  }
}

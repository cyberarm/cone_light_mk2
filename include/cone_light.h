#pragma once

#include "cone_light_constants.h"

#include <Preferences.h>
#include "cone_light_enums.h"
#include "cone_light_command_handler.h"
#include "cone_light_input_handler.h"
#include "cone_light_lighting.h"
#include "cone_light_display.h"
#include "cone_light_speaker.h"
#include "cone_light_voltage.h"
#include "cone_light_application.h"
#include "cone_light_networking.h"
#include "cone_light_network_time.h"

// Forward declarations... 💔 you C 😿
class ConeLightCommandHandler;
class ConeLightInputHandler;
class ConeLightLighting;
class ConeLightDisplay;
class ConeLightSpeaker;
class ConeLightVoltage;
class ConeLightApplication;
class ConeLightNetworking;
class ConeLightNetworkTime;
typedef struct cone_light_network_packet cone_light_network_packet_t;

class ConeLight
{
private:
  ConeLightCommandHandler *m_command_handler = nullptr;
  ConeLightInputHandler *m_input_handler = nullptr;
  ConeLightLighting *m_lighting = nullptr;
  ConeLightDisplay *m_display = nullptr;
  ConeLightSpeaker *m_speaker = nullptr;
  ConeLightVoltage *m_voltage = nullptr;
  ConeLightNetworking *m_networking = nullptr;
  ConeLightNetworkTime *m_network_time = nullptr;
  ConeLightApplication *m_current_app = nullptr;
  std::vector<ConeLightApplication *> m_applications = {};
  Preferences m_preferences;
  uint8_t m_node_id = CONE_LIGHT_NODE_ID_UNSET;
  uint8_t m_node_group = CONE_LIGHT_NODE_GROUP_ID_UNSET;
  String m_node_name = CONE_LIGHT_NODE_NAME_UNSET;
  bool m_node_grandmaster_clock = false;
  uint32_t m_last_input_change_ms = 0;
  bool m_screensaver = false;
  bool m_muted = CONE_LIGHT_MUTED;

public:
  ConeLight();
  ~ConeLight();
  void update();
  uint8_t node_id();
  uint8_t node_group_id();
  String node_name();
  bool node_grandmaster_clock();
  bool reconfigure_node(uint8_t node_id, uint8_t node_group, String node_name, bool node_grandmaster_clock);
  void boot_complete();
  bool screensaver() { return m_screensaver; };
  void update_screensaver();
  bool muted() { return m_muted; };
  void set_muted(bool mute) { m_muted = mute; };
  ConeLightApplication *current_app() { return m_current_app; };
  void set_current_app(ConeLightApplication *app) { m_current_app = app; };
  void set_current_app_main_menu();
  std::vector<ConeLightApplication *> applications() { return m_applications; };
  void button_event(ConeLightButton btn, ConeLightEvent state);
  void lid_event(ConeLightEvent state);
  void espnow_event(cone_light_network_packet_t packet);

  ConeLightCommandHandler *command_handler() { return m_command_handler; }
  ConeLightInputHandler *input_handler() { return m_input_handler; }
  ConeLightLighting *lighting() { return m_lighting; }
  ConeLightDisplay *display() { return m_display; }
  ConeLightSpeaker *speaker() { return m_speaker; }
  ConeLightVoltage *voltage() { return m_voltage; }
  ConeLightNetworking *networking() { return m_networking; }
  ConeLightNetworkTime *network_time() { return m_network_time; }
};

#pragma once

// #define NODE_ID 0x00
// #define NODE_GROUP "BACKSTOP" // "STOPSIGN"
// #define NODE_NAME "KETTLE" // HOBBIT FIDDLE DRAGON FLOWER LOCKET MIRROR PILLOW

#define CONE_LIGHT_DEBUG false
#define CONE_LIGHT_ALLOW_PHOTOSENSITIVE_HAZARDS false

#define CONE_LIGHT_PRODUCT_NAME "Cone Light mk. II"
// Whether buttons cause beeps and chirps
#define CONE_LIGHT_MUTED false
// Play 4 notes on boot up
#define CONE_LIGHT_BOOT_UP_TUNE true
// Whether node's attempt to sync with the grand master clock node
#define CONE_LIGHT_GRAND_MASTER_CLOCK_ENABLED false

#define CONE_LIGHT_NODE_ID_UNSET 255
#define CONE_LIGHT_NODE_GROUP_ID_UNSET 255
#define CONE_LIGHT_NODE_NAME_UNSET "NONAME"
#define CONE_LIGHT_NODE_GRAND_MASTER_CLOCK_UNSET_OR_FALSE false

#define CONE_LIGHT_NODE_GROUP_0 0
#define CONE_LIGHT_NODE_GROUP_0_NAME "BACKSTOP"
#define CONE_LIGHT_NODE_GROUP_1 1
#define CONE_LIGHT_NODE_GROUP_1_NAME "STOPSIGN"
#define CONE_LIGHT_NODE_GROUP_255 255
#define CONE_LIGHT_NODE_GROUP_255_NAME "!NOT SET"

#define CONE_LIGHT_PREFERENCES_ID "cone_light"
#define CONE_LIGHT_PREFERENCES_NODE_ID "node_id"
#define CONE_LIGHT_PREFERENCES_NODE_GROUP "node_group"
#define CONE_LIGHT_PREFERENCES_NODE_NAME "node_name"
#define CONE_LIGHT_PREFERENCES_NODE_GRANDMASTER_CLOCK "node_clock"

// Updated whenever changes are made. YYYY.MM.DD
#define CONE_LIGHT_FIRMWARE_VERSION_NAME "2025.07.18"
// Used for backward incompatible changes
#define CONE_LIGHT_FIRMWARE_VERSION 1

#define BTN_A_PIN D10
#define BTN_B_PIN D9
#define BTN_C_PIN D8
#define SPEAKER_PIN D2
#define LID_PIN D1
#define VOLTAGE_PIN A0 // On D0

#define BTN_HELD_AFTER_MS 500
#define BTN_DEBOUNCE_MS 50

#define LID_DEBOUNCE_MS 500

#define BTN_PRESSED_SPEAKER_CHIRP_FREQUENCY 440
#define BTN_HELD_SPEAKER_CHIRP_FREQUENCY 1760
#define BTN_RELEASED_SPEAKER_CHIRP_FREQUENCY 110
#define BTN_SPEAKER_CHIRP_DURATION 50

#define VOLTAGE_MAX 5.6f
#define VOLTAGE_MIN 4.72f

#define SCREENSAVER_TIMEOUT_MS 60'000

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

#include <Preferences.h>
#include "cone_light.h"

ConeLight::ConeLight()
{
  Serial.begin();
  m_preferences.begin(CONE_LIGHT_PREFERENCES_ID);

  Serial.printf("Initializing %s v%s (internal v%d)\n", CONE_LIGHT_PRODUCT_NAME, CONE_LIGHT_FIRMWARE_VERSION_NAME, CONE_LIGHT_FIRMWARE_VERSION);
  Serial.println("  Configuring button inputs...");
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(BTN_C_PIN, INPUT_PULLUP);

  pinMode(LID_PIN, INPUT_PULLUP);

  // Load node specific data from Preferences
  Serial.println("  Loading node preferences data...");
  m_node_id = m_preferences.getUChar(CONE_LIGHT_PREFERENCES_NODE_ID, CONE_LIGHT_NODE_ID_UNSET);
  m_node_group = m_preferences.getUChar(CONE_LIGHT_PREFERENCES_NODE_GROUP, CONE_LIGHT_NODE_GROUP_UNSET);
  m_node_name = m_preferences.getString(CONE_LIGHT_PREFERENCES_NODE_NAME, CONE_LIGHT_NODE_NAME_UNSET);

  // Initialize subsystems...
  Serial.println("  Initializing subsystems...");
  m_command_handler = new ConeLightCommandHandler(this);
  m_lighting = new ConeLightLighting(this);
  m_display = new ConeLightDisplay(this);
  m_speaker = new ConeLightSpeaker(this);

  Serial.printf("Initialization of node %s (id: %d, group: %d) completed.\n", m_node_name.c_str(), m_node_id, m_node_group);
}

ConeLight::~ConeLight()
{
  delete m_speaker;
  delete m_display;
  delete m_lighting;
  delete m_command_handler;

  m_preferences.end();
  Serial.end();
}

void ConeLight::update()
{
  m_command_handler->update();
  m_lighting->update();
  m_display->update();
  m_speaker->update();
}

uint8_t ConeLight::node_id()
{
  return m_node_id;
}

uint8_t ConeLight::node_group()
{
  return m_node_group;
}

String ConeLight::node_name()
{
  return m_node_name;
}

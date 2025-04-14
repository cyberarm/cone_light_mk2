#include "cone_light_lighting.h"

ConeLightLighting::ConeLightLighting(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  // Set default color based on node group
  // GOLD for group 0, TEAL for group 1, and PURPLE for not configured.
  m_color = (m_cone_light->node_group() == CONE_LIGHT_NODE_GROUP_0) ? COLOR_GOLD : (m_cone_light->node_group() == CONE_LIGHT_NODE_GROUP_1) ?  COLOR_TEAL : COLOR_PURPLE;

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(m_leds, NUM_LEDS).setCorrection(UncorrectedColor);
  FastLED.setBrightness(m_brightness);
  FastLED.clear(true);

  FastLED.showColor(CRGB(0, 0, 0));
}

ConeLightLighting::~ConeLightLighting()
{
}

void ConeLightLighting::update()
{
  if (millis() - m_last_milliseconds < 667)
    m_brightness = 2;
  else if (millis() - m_last_milliseconds < (667 + 32) * 2)
    m_brightness = 4;
  else if (millis() - m_last_milliseconds < (667 + 32 + 32) * 3)
    m_brightness = 6;
  else if (millis() - m_last_milliseconds < (667 + 32 + 32 + 32) * 4)
    m_brightness = 8;
  else if (!m_boot_lighting_shown)
    m_boot_lighting_shown = true;

  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    m_leds[i] = m_boot_lighting_shown ? m_color : m_boot_color;
  }

  FastLED.show(m_brightness);
}

void ConeLightLighting::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
  m_color = CRGB(red, green, blue);
}

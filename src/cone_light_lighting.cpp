#include "cone_light_lighting.h"

ConeLightLighting::ConeLightLighting(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(m_leds, NUM_LEDS).setCorrection(UncorrectedColor);
  FastLED.setBrightness(m_brightness);
  FastLED.clear(true);

  FastLED.showColor(CRGB(0, 0, 0));

  Serial.println("    FastLED initialized successfully.");
}

ConeLightLighting::~ConeLightLighting()
{
}

void ConeLightLighting::update()
{
  if (m_needs_redraw)
  {
    m_needs_redraw = false;

    for (size_t i = 0; i < NUM_LEDS; i++)
    {
      m_leds[i] = m_color;
    }

    FastLED.show(m_brightness);
  }
}

void ConeLightLighting::set_color(uint8_t red, uint8_t green, uint8_t blue)
{
  m_color = CRGB(red, green, blue);
  m_needs_redraw = true;
}

void ConeLightLighting::set_color(CRGB color)
{
  m_color = color;
  m_needs_redraw = true;
}

void ConeLightLighting::set_static_color(uint8_t red, uint8_t green, uint8_t blue)
{
  m_static_color = CRGB(red, green, blue);
  m_needs_redraw = true;
}

void ConeLightLighting::set_static_color(CRGB color)
{
  m_static_color = color;
  m_needs_redraw = true;
}

void ConeLightLighting::set_brightness(uint8_t brightness)
{
  m_brightness = brightness;
  m_needs_redraw = true;
}

void ConeLightLighting::set_static_brightness(uint8_t brightness)
{
  m_static_brightness = brightness;
  m_needs_redraw = true;
}

void ConeLightLighting::handle_packet(cone_light_network_packet_t packet)
{
  // Ignore packets meant for another group/cluster of nodes
  if (packet.command_type == ConeLightNetworkCommand::SET_GROUP_COLOR &&
      packet.command_parameters_extra != m_cone_light->node_group_id())
    return;

  // Only apply selected color if a song is NOT playing to PREVENT strobing between led animation color and chosen color
  if (!m_cone_light->speaker()->playing())
    set_color(CRGB(packet.command_parameters));
  set_static_color(CRGB(packet.command_parameters));
  set_brightness((packet.command_parameters >> 24) & 0xFF);
  set_static_brightness((packet.command_parameters >> 24) & 0xFF);
}

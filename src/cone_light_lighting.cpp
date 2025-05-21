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

void ConeLightLighting::set_brightness(uint8_t brightness)
{
  m_brightness = brightness;
  m_needs_redraw = true;
}

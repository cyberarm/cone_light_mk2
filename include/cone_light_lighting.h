#pragma once

#include "cone_light.h"
#include <FastLED.h>

#define LED_PIN D3
#define NUM_LEDS 6
#define LED_TYPE WS2812B
#define COLOR_ORDER RGB

#define COLOR_GOLD CRGB(255, 127, 0)
#define COLOR_TEAL CRGB(0, 127, 255)
#define COLOR_PURPLE CRGB(255, 0, 255)

// Forward declaration...
class ConeLight;

class ConeLightLighting
{
private:
  CRGB m_leds[NUM_LEDS];
  CRGB m_color = COLOR_PURPLE;
  uint16_t m_brightness = 8;
  bool m_needs_redraw = true;
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightLighting(ConeLight *cone_light);
  ~ConeLightLighting();
  void update();
  void set_color(uint8_t red, uint8_t green, uint8_t blue);
  void set_color(CRGB color);
  void set_brightness(uint8_t brightness);
};

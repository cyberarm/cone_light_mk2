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
  CRGB m_boot_color = CRGB(83, 151, 152);
  CRGB m_color = COLOR_PURPLE;
  uint16_t m_brightness = 8;
  unsigned long m_last_milliseconds = millis();
  bool m_boot_lighting_shown = false;
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightLighting(ConeLight *cone_light);
  ~ConeLightLighting();
  void update();
  void set_color(uint8_t red, uint8_t green, uint8_t blue);
  void set_brightness(uint8_t brightness);
};

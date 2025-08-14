#pragma once

#include "cone_light.h"
#include <FastLED.h>

#define LED_PIN D3
#define NUM_LEDS 6
#define LED_TYPE WS2812B
#define COLOR_ORDER RGB

#define COLOR_GOLD CRGB(255, 102, 0)
#define COLOR_TEAL CRGB(0, 255, 255)
#define COLOR_PURPLE CRGB(255, 0, 255)

#define LED_DEFAULT_BRIGHTNESS 20

// Forward declaration...
class ConeLight;
typedef struct cone_light_network_packet cone_light_network_packet_t;

constexpr CRGB notes_to_color[] = {
    CRGB(4278255487),
    CRGB(4278222847),
    CRGB(4286578432),
    CRGB(4278255360),
    CRGB(4294934272),
    CRGB(4294901760),
    CRGB(4294901887),
    CRGB(4294934272),
    CRGB(4278190335),
    CRGB(4278222847),
    CRGB(4278255615),
    CRGB(4278255487),
};

class ConeLightLighting
{
private:
  CRGB m_leds[NUM_LEDS];
  CRGB m_color = COLOR_PURPLE;
  CRGB m_static_color = COLOR_PURPLE;
  uint8_t m_brightness = LED_DEFAULT_BRIGHTNESS;
  uint8_t m_static_brightness = LED_DEFAULT_BRIGHTNESS;
  bool m_needs_redraw = true;
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightLighting(ConeLight *cone_light);
  ~ConeLightLighting();
  void update();
  void set_color(uint8_t red, uint8_t green, uint8_t blue);
  void set_color(CRGB color);
  // static color is the color the light should return to after an animation or other automated led controller completes
  void set_static_color(uint8_t red, uint8_t green, uint8_t blue);
  void set_static_color(CRGB color);
  // static brightness is the brightness the light should use while an led animation is running and return to after an animation or other automated led controller completes
  void set_brightness(uint8_t brightness);
  void set_static_brightness(uint8_t brightness);

  CRGB get_color() { return m_color; };
  CRGB get_static_color() { return m_static_color; };
  uint8_t get_brightness() { return m_brightness; };
  uint8_t get_static_brightness() { return m_static_brightness; };
  CRGB note_to_color(int8_t note)
  {
    if (note < 0)
      return CRGB::Black;

    return notes_to_color[note % 12];
  };

  void handle_packet(cone_light_network_packet_t packet);
};

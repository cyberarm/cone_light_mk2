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

const std::unordered_map<uint16_t, CRGB> frequencies_to_color =
    {
        // Inaudible tones (Speaker / ledcWriteTone cannot produce these frequencies [as configured])
        {0, CRGB::Black},
        {16, CRGB::Black},
        {17, CRGB::Black},
        {18, CRGB::Black},
        {19, CRGB::Black},
        {21, CRGB::Black},
        {22, CRGB::Black},
        {23, CRGB::Black},
        {24, CRGB::Black},
        {26, CRGB::Black},
        {28, CRGB::Black},
        {29, CRGB::Black},
        {31, CRGB::Black},
        {33, CRGB::Black},
        {35, CRGB::Black},
        {37, CRGB::Black},
        // Audible tones
        {39, CRGB(4278255487)},
        {41, CRGB(4278255360)},
        {44, CRGB(4286578432)},
        {46, CRGB(4294967040)},
        {49, CRGB(4294934272)},
        {52, CRGB(4294936576)},
        {55, CRGB(4294901887)},
        {58, CRGB(4294967040)},
        {62, CRGB(4294934272)},
        {65, CRGB(4278190335)},
        {69, CRGB(4278222847)},
        {73, CRGB(4278255615)},
        {78, CRGB(4278255487)},
        {82, CRGB(4278255360)},
        {87, CRGB(4286578432)},
        {92, CRGB(4294967040)},
        {98, CRGB(4294934272)},
        {104, CRGB(4294936576)},
        {110, CRGB(4294901887)},
        {117, CRGB(4294967040)},
        {123, CRGB(4294934272)},
        {131, CRGB(4278190335)},
        {139, CRGB(4278222847)},
        {147, CRGB(4278255615)},
        {156, CRGB(4278255487)},
        {165, CRGB(4278255360)},
        {175, CRGB(4286578432)},
        {185, CRGB(4294967040)},
        {196, CRGB(4294934272)},
        {208, CRGB(4294936576)},
        {220, CRGB(4294901887)},
        {233, CRGB(4294967040)},
        {247, CRGB(4294934272)},
        {262, CRGB(4278190335)},
        {277, CRGB(4278222847)},
        {294, CRGB(4278255615)},
        {311, CRGB(4278255487)},
        {330, CRGB(4278255360)},
        {349, CRGB(4286578432)},
        {370, CRGB(4294967040)},
        {392, CRGB(4294934272)},
        {415, CRGB(4294936576)},
        {440, CRGB(4294901887)},
        {466, CRGB(4294967040)},
        {494, CRGB(4294934272)},
        {523, CRGB(4278190335)},
        {554, CRGB(4278222847)},
        {587, CRGB(4278255615)},
        {622, CRGB(4278255487)},
        {659, CRGB(4278255360)},
        {698, CRGB(4286578432)},
        {740, CRGB(4294967040)},
        {784, CRGB(4294934272)},
        {831, CRGB(4294936576)},
        {880, CRGB(4294901887)},
        {932, CRGB(4294967040)},
        {988, CRGB(4294934272)},
        {1047, CRGB(4278190335)},
        {1109, CRGB(4278222847)},
        {1175, CRGB(4278255615)},
        {1245, CRGB(4278255487)},
        {1319, CRGB(4278255360)},
        {1397, CRGB(4286578432)},
        {1480, CRGB(4294967040)},
        {1568, CRGB(4294934272)},
        {1661, CRGB(4294936576)},
        {1760, CRGB(4294901887)},
        {1865, CRGB(4294967040)},
        {1976, CRGB(4294934272)},
        {2093, CRGB(4278190335)},
        {2217, CRGB(4278222847)},
        {2349, CRGB(4278255615)},
        {2489, CRGB(4278255487)},
        {2637, CRGB(4278255360)},
        {2794, CRGB(4286578432)},
        {2960, CRGB(4294967040)},
        {3136, CRGB(4294934272)},
        {3322, CRGB(4294936576)},
        {3520, CRGB(4294901887)},
        {3729, CRGB(4294967040)},
        {3951, CRGB(4294934272)},
        {4186, CRGB(4278190335)},
        {4435, CRGB(4278222847)},
        {4699, CRGB(4278255615)},
        {4978, CRGB(4278255487)},
        {5274, CRGB(4278255360)},
        {5588, CRGB(4286578432)},
        {5920, CRGB(4294967040)},
        {6272, CRGB(4294934272)},
        {6645, CRGB(4294936576)},
        {7040, CRGB(4294901887)},
        {7459, CRGB(4294967040)},
        {7902, CRGB(4294934272)},
        {8372, CRGB(4278190335)},
        {8870, CRGB(4278222847)},
        {9397, CRGB(4278255615)},
        {9956, CRGB(4278255487)},
        {10548, CRGB(4278255360)},
        {11175, CRGB(4286578432)},
        {11840, CRGB(4294967040)},
        {12544, CRGB(4294934272)},
        {13290, CRGB(4294936576)},
        {14080, CRGB(4294901887)},
        {14917, CRGB(4294967040)},
        {15804, CRGB(4294934272)}
};

class ConeLightLighting
{
private:
  CRGB m_leds[NUM_LEDS];
  CRGB m_color = COLOR_PURPLE;
  CRGB m_static_color = COLOR_PURPLE;
  uint8_t m_brightness = LED_DEFAULT_BRIGHTNESS;
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
  void set_brightness(uint8_t brightness);

  CRGB get_color() { return m_color; };
  CRGB get_static_color() { return m_static_color; };
  uint8_t get_brightness() { return m_brightness; };
  CRGB frequency_to_color(uint16_t frequency)
  {
    if (frequencies_to_color.contains(frequency))
      return frequencies_to_color.at(frequency);

    return CRGB::Black;
  };

  void handle_packet(cone_light_network_packet_t packet);
};

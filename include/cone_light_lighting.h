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
        {0, CRGB(145, 65, 172)},
        {16, CRGB(153, 193, 241)},
        {17, CRGB(53, 132, 228)},
        {18, CRGB(143, 240, 164)},
        {19, CRGB(51, 209, 122)},
        {21, CRGB(249, 240, 107)},
        {22, CRGB(246, 211, 45)},
        {23, CRGB(255, 190, 111)},
        {24, CRGB(255, 120, 0)},
        {26, CRGB(246, 97, 81)},
        {28, CRGB(224, 27, 36)},
        {29, CRGB(220, 138, 221)},
        {31, CRGB(145, 65, 172)},
        {33, CRGB(153, 193, 241)},
        {35, CRGB(53, 132, 228)},
        {37, CRGB(143, 240, 164)},
        {39, CRGB(51, 209, 122)},
        {41, CRGB(249, 240, 107)},
        {44, CRGB(246, 211, 45)},
        {46, CRGB(255, 190, 111)},
        {49, CRGB(255, 120, 0)},
        {52, CRGB(246, 97, 81)},
        {55, CRGB(224, 27, 36)},
        {58, CRGB(220, 138, 221)},
        {62, CRGB(145, 65, 172)},
        {65, CRGB(153, 193, 241)},
        {69, CRGB(53, 132, 228)},
        {73, CRGB(143, 240, 164)},
        {78, CRGB(51, 209, 122)},
        {82, CRGB(249, 240, 107)},
        {87, CRGB(246, 211, 45)},
        {92, CRGB(255, 190, 111)},
        {98, CRGB(255, 120, 0)},
        {104, CRGB(246, 97, 81)},
        {110, CRGB(224, 27, 36)},
        {117, CRGB(220, 138, 221)},
        {123, CRGB(145, 65, 172)},
        {131, CRGB(153, 193, 241)},
        {139, CRGB(53, 132, 228)},
        {147, CRGB(143, 240, 164)},
        {156, CRGB(51, 209, 122)},
        {165, CRGB(249, 240, 107)},
        {175, CRGB(246, 211, 45)},
        {185, CRGB(255, 190, 111)},
        {196, CRGB(255, 120, 0)},
        {208, CRGB(246, 97, 81)},
        {220, CRGB(224, 27, 36)},
        {233, CRGB(220, 138, 221)},
        {247, CRGB(145, 65, 172)},
        {262, CRGB(153, 193, 241)},
        {277, CRGB(53, 132, 228)},
        {294, CRGB(143, 240, 164)},
        {311, CRGB(51, 209, 122)},
        {330, CRGB(249, 240, 107)},
        {349, CRGB(246, 211, 45)},
        {370, CRGB(255, 190, 111)},
        {392, CRGB(255, 120, 0)},
        {415, CRGB(246, 97, 81)},
        {440, CRGB(224, 27, 36)},
        {466, CRGB(220, 138, 221)},
        {494, CRGB(145, 65, 172)},
        {523, CRGB(153, 193, 241)},
        {554, CRGB(53, 132, 228)},
        {587, CRGB(143, 240, 164)},
        {622, CRGB(51, 209, 122)},
        {659, CRGB(249, 240, 107)},
        {698, CRGB(246, 211, 45)},
        {740, CRGB(255, 190, 111)},
        {784, CRGB(255, 120, 0)},
        {831, CRGB(246, 97, 81)},
        {880, CRGB(224, 27, 36)},
        {932, CRGB(220, 138, 221)},
        {988, CRGB(145, 65, 172)},
        {1047, CRGB(153, 193, 241)},
        {1109, CRGB(53, 132, 228)},
        {1175, CRGB(143, 240, 164)},
        {1245, CRGB(51, 209, 122)},
        {1319, CRGB(249, 240, 107)},
        {1397, CRGB(246, 211, 45)},
        {1480, CRGB(255, 190, 111)},
        {1568, CRGB(255, 120, 0)},
        {1661, CRGB(246, 97, 81)},
        {1760, CRGB(224, 27, 36)},
        {1865, CRGB(220, 138, 221)},
        {1976, CRGB(145, 65, 172)},
        {2093, CRGB(153, 193, 241)},
        {2217, CRGB(53, 132, 228)},
        {2349, CRGB(143, 240, 164)},
        {2489, CRGB(51, 209, 122)},
        {2637, CRGB(249, 240, 107)},
        {2794, CRGB(246, 211, 45)},
        {2960, CRGB(255, 190, 111)},
        {3136, CRGB(255, 120, 0)},
        {3322, CRGB(246, 97, 81)},
        {3520, CRGB(224, 27, 36)},
        {3729, CRGB(220, 138, 221)},
        {3951, CRGB(145, 65, 172)},
        {4186, CRGB(153, 193, 241)},
        {4435, CRGB(53, 132, 228)},
        {4699, CRGB(143, 240, 164)},
        {4978, CRGB(51, 209, 122)},
        {5274, CRGB(249, 240, 107)},
        {5588, CRGB(246, 211, 45)},
        {5920, CRGB(255, 190, 111)},
        {6272, CRGB(255, 120, 0)},
        {6645, CRGB(246, 97, 81)},
        {7040, CRGB(224, 27, 36)},
        {7459, CRGB(220, 138, 221)},
        {7902, CRGB(145, 65, 172)},
        {8372, CRGB(153, 193, 241)},
        {8870, CRGB(53, 132, 228)},
        {9397, CRGB(143, 240, 164)},
        {9956, CRGB(51, 209, 122)},
        {10548, CRGB(249, 240, 107)},
        {11175, CRGB(246, 211, 45)},
        {11840, CRGB(255, 190, 111)},
        {12544, CRGB(255, 120, 0)},
        {13290, CRGB(246, 97, 81)},
        {14080, CRGB(224, 27, 36)},
        {14917, CRGB(220, 138, 221)},
        {15804, CRGB(145, 65, 172)}
};

class ConeLightLighting
{
private:
  CRGB m_leds[NUM_LEDS];
  CRGB m_color = COLOR_PURPLE;
  uint8_t m_brightness = LED_DEFAULT_BRIGHTNESS;
  bool m_needs_redraw = true;
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightLighting(ConeLight *cone_light);
  ~ConeLightLighting();
  void update();
  void set_color(uint8_t red, uint8_t green, uint8_t blue);
  void set_color(CRGB color);
  void set_brightness(uint8_t brightness);

  CRGB get_color() { return m_color; };
  uint8_t get_brightness() { return m_brightness; };
  CRGB frequency_to_color(uint16_t frequency)
  {
    if (frequencies_to_color.contains(frequency))
      return frequencies_to_color.at(frequency);

    return CRGB();
  };

  void handle_packet(cone_light_network_packet_t packet);
};

#pragma once

#include <Arduino.h>

#include "cone_light_constants.h"

// forward declaration...
class ConeLight;

class ConeLightAmbientLight
{
private:
  constexpr static uint8_t m_ambient_light_samples = 16;
  constexpr static uint8_t m_ambient_light_history_samples = 60;

  ConeLight *m_cone_light = nullptr;
  float m_ambient_light = 0.0f;
  std::array<float, m_ambient_light_history_samples> m_history = {0.0f};
  uint8_t m_history_index = 0;
  uint32_t m_last_updated_ms = 0;
  uint32_t m_update_interval_ms = 3'000;

public:
  ConeLightAmbientLight(ConeLight *cone_light);
  ~ConeLightAmbientLight();
  void update();
  void reset();
  float read_ambient_light();
  float ambient_light();
  float ambient_light_average();
  float ambient_light_percentage();
  float ambient_light_average_percentage();
  float ambient_light_percentage(float ambient_light);
};

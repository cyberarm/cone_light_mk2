#pragma once

#include <Arduino.h>

class ConeLightVoltage
{
private:
  ConeLight *m_cone_light = nullptr;
  float m_voltage = 3.3f;
  constexpr static uint8_t m_voltage_samples = 16;
  uint32_t m_last_updated_ms = 0;
  uint32_t m_update_interval_ms = 3'000;

public:
  ConeLightVoltage(ConeLight *cone_light);
  ~ConeLightVoltage();
  void update();
  void reset();
  float read_voltage();
  float voltage();
  float voltage_percentage();
  float voltage_percentage(float voltage);
};

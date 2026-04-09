#include "include/cone_light_ambient_light.h"

ConeLightAmbientLight::ConeLightAmbientLight(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  pinMode(LDR_PIN, INPUT);

  Serial.println("    Ambient Light initialized successfully.");
}

ConeLightAmbientLight::~ConeLightAmbientLight()
{
}

void ConeLightAmbientLight::update()
{
  if (millis() - m_last_updated_ms >= m_update_interval_ms)
  {
    m_last_updated_ms = millis();

    m_ambient_light = read_ambient_light();

    // store history in circular buffer
    m_history[m_history_index] = m_ambient_light;
    m_history_index = (m_history_index + 1) % m_history.size();

    // Serial.printf("AMBIENT LIGHT: %.3fv (%3.1f%%)\n", ambient_light(), ambient_light_percentage());
  }
}

void ConeLightAmbientLight::reset()
{
}

float ConeLightAmbientLight::read_ambient_light()
{
  uint32_t ambient_light_sampler = 0;
  for (uint8_t i = 0; i < m_ambient_light_samples; i++)
  {
    ambient_light_sampler += analogReadMilliVolts(LDR_PIN);
  }

  return ((ambient_light_sampler / m_ambient_light_samples) / 1000.0);
}

float ConeLightAmbientLight::ambient_light()
{
  return m_ambient_light;
}

float ConeLightAmbientLight::ambient_light_average()
{
  double ambient_light_avg = 0.0f;
  uint8_t samples = 0;

  for (const float v : m_history)
  {
    // remove zero bias from average.
    // analog inputs are (almost) never zero.
    if (v == 0.0f)
      break;

    ambient_light_avg += v;

    samples++;
  }

  // if every sample is zero, then return zero.
  // prevent divide by zero.
  if (samples == 0)
    return 0.0f;

  return ambient_light_avg / samples;
}

float ConeLightAmbientLight::ambient_light_percentage()
{
  return ambient_light_percentage(m_ambient_light);
}

float ConeLightAmbientLight::ambient_light_average_percentage()
{
  return ambient_light_percentage(ambient_light_average());
}

float ConeLightAmbientLight::ambient_light_percentage(float ambient_light)
{
  return std::clamp(ambient_light / 3.3f, 0.0f, 1.0f) * 100.0f;
}

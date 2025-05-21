#include "cone_light.h"
#include "cone_light_voltage.h"

ConeLightVoltage::ConeLightVoltage(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  pinMode(VOLTAGE_PIN, INPUT);

  Serial.println("    Voltage initialized successfully.");
}

ConeLightVoltage::~ConeLightVoltage()
{
}

void ConeLightVoltage::update()
{
  if (millis() - m_last_updated_ms >= m_update_interval_ms)
  {
    m_last_updated_ms = millis();

    m_voltage = read_voltage();

    // Serial.printf("VOLTAGE: %.3fv (%3.1f%%)\n", voltage(), voltage_percentage());
  }
}

void ConeLightVoltage::reset()
{
}

float ConeLightVoltage::read_voltage()
{
  uint32_t voltage_sampler = 0;
  for (int i = 0; i < m_voltage_samples; i++)
  {
    voltage_sampler += analogReadMilliVolts(VOLTAGE_PIN);
  }

  return (2 * ((voltage_sampler / m_voltage_samples) / 1000.0));
}

float ConeLightVoltage::voltage()
{
  return m_voltage;
}

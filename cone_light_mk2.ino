#include <Arduino.h>
#include "cone_light.h"
#include "cyberarm_song.h"

ConeLight *m_cone_light = nullptr;
void setup()
{
  m_cone_light = new ConeLight();
}

void loop()
{
  m_cone_light->update();
}

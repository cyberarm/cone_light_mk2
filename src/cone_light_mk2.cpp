#include <Arduino.h>
#include "cone_light.h"
#include "cyberarm_song.h"

ConeLight *g_cone_light = nullptr;
void setup()
{
  g_cone_light = new ConeLight();
}

void loop()
{
  g_cone_light->update();
}

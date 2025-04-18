#pragma once

#include "cone_light.h"

// Forward declaration...
class ConeLight;

class ConeLightCommandHandler
{
private:
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightCommandHandler(ConeLight *cone_light);
  ~ConeLightCommandHandler();
  void update();
  void reset();
};

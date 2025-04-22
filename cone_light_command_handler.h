#pragma once

#include "cone_light.h"
#include "cone_light_command.h"

// Forward declaration...
class ConeLight;
class ConeLightCommand;

// --- ConeLightConeLightCommandHandler --- //
class ConeLightCommandHandler
{
private:
  ConeLight *m_cone_light = nullptr;
  std::vector<ConeLightCommand*> m_commands = {};

public:
  ConeLightCommandHandler(ConeLight *cone_light);
  ~ConeLightCommandHandler();
  void update();
  void reset();
  void register_command(ConeLightCommand command);
  std::vector<ConeLightCommand*> commands() { return m_commands; };
};

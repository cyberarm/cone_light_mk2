#pragma once

#define SPEAKER D2

#include "cone_light.h"
#include "cyberarm_song.h"

// Forward declaration...
class ConeLight;

class ConeLightSpeaker
{
private:
  CyberarmSong *m_song = new CyberarmSong();
  unsigned long m_last_milliseconds = millis();
  bool m_boot_sound_played = false;
  ConeLight* m_cone_light = nullptr;

public:
  ConeLightSpeaker(ConeLight* cone_light);
  ~ConeLightSpeaker();
  void update();
  void reset();
};

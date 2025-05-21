#pragma once

#include "cone_light.h"
#include "cyberarm_song.h"

// Forward declaration...
class ConeLight;
class ConeLightSong;

class ConeLightSpeaker
{
private:
  CyberarmSong *m_song = new CyberarmSong();
  unsigned long m_last_milliseconds = millis();
  bool m_speaker_inited = false;
  bool m_boot_sound_played = false;
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightSpeaker(ConeLight *cone_light);
  ~ConeLightSpeaker();
  void update();
  void reset();
  void play_song(uint16_t song_id);
  void play_tone(uint16_t frequency, uint16_t duration);
};

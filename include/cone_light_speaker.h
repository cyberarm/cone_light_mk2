#pragma once

#include "cone_light.h"
#include "cyberarm_song.h"
#include <FastLED.h>
#include <Tween.h>

// Forward declaration...
class ConeLight;
class ConeLightSong;
typedef struct cone_light_network_packet cone_light_network_packet_t;

class ConeLightSpeaker
{
private:
  CyberarmSong *m_song = new CyberarmSong();
  uint32_t m_last_milliseconds = millis();
  bool m_speaker_inited = false;
  ConeLight *m_cone_light = nullptr;
  int16_t m_current_channel_note_id = -1;
  bool m_song_playing = false;
  Tween::Timeline m_led_timeline;
  CRGB m_led_song_color = {};
  std::vector<int16_t> m_notes = {0};
  std::vector<uint16_t> m_durations = {0};

public:
  ConeLightSpeaker(ConeLight *cone_light);
  ~ConeLightSpeaker();
  void update();
  void reset();
  void play_song(uint16_t song_id);
  void play_tone(uint16_t frequency, uint16_t duration);
  bool playing() { return m_song->playing(); };

  void handle_packet(cone_light_network_packet_t packet);
  void animate_leds_with_song();
};

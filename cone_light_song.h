#pragma once

#include <Arduino.h>
// #include "cyberarm_song.h"

// struct cone_light_song_t
// {
//   String name;
//   int16_t notes[MAX_CHANNELS][MAX_NOTES];
//   int16_t durations[MAX_CHANNELS][MAX_NOTES];

//   int16_t note_count(int16_t channel)
//   {
//     return notes[channel];
//   }
// } cone_light_song_t;

class ConeLightSong
{
private:
  String m_name;
  std::vector<std::vector<int16_t>> m_notes;
  std::vector<std::vector<uint16_t>> m_durations;

public:
  ConeLightSong(String name, std::vector<std::vector<int16_t>> notes, std::vector<std::vector<uint16_t>> durations);
  ~ConeLightSong();
  String name();
  std::vector<int16_t> channel_notes(uint8_t channel_id);
  std::vector<uint16_t> channel_durations(uint8_t channel_id);
};

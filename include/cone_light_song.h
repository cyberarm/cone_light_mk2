#pragma once

#include <Arduino.h>
#include "cyberarm_song.h"

class ConeLightSong
{
private:
  String m_name;
  const std::vector<std::vector<int8_t>> m_notes;
  const std::vector<std::vector<uint16_t>> m_durations;

public:
  ConeLightSong(const String name, const std::vector<std::vector<int8_t>> notes, const std::vector<std::vector<uint16_t>> durations) : m_notes(notes), m_durations(durations)
  {
    m_name = name;
  };
  ~ConeLightSong() {};
  String name() const;
  std::vector<int8_t> channel_notes(uint8_t channel_id) const;
  std::vector<uint16_t> channel_durations(uint8_t channel_id) const;
};

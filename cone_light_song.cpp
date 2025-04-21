#include "cone_light_song.h"

ConeLightSong::ConeLightSong(String name, std::vector<std::vector<int16_t>> notes, std::vector<std::vector<uint16_t>> durations)
{
  m_name = name;
  m_notes = notes;
  m_durations = durations;
}

ConeLightSong::~ConeLightSong()
{
}

String ConeLightSong::name()
{
  return m_name;
}

std::vector<int16_t> ConeLightSong::channel_notes(uint8_t channel_id)
{
  if (m_notes.size() < channel_id)
    return m_notes[channel_id];
  else
    return {};
}

std::vector<uint16_t> ConeLightSong::channel_durations(uint8_t channel_id)
{
  if (m_durations.size() < channel_id)
    return m_durations[channel_id];
  else
    return {};
}

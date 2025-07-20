#include "cone_light_song.h"

ConeLightSong::ConeLightSong(const String name, const std::vector<std::vector<int16_t>> notes, const std::vector<std::vector<uint16_t>> durations)
{
  m_name = name;
  m_notes = notes;
  m_durations = durations;
}

ConeLightSong::~ConeLightSong()
{
}

String ConeLightSong::name() const
{
  return m_name;
}

std::vector<int16_t> ConeLightSong::channel_notes(uint8_t channel_id) const
{
  if (channel_id < m_notes.size())
    return m_notes[channel_id];
  else
    return {};
}

std::vector<uint16_t> ConeLightSong::channel_durations(uint8_t channel_id) const
{
  if (channel_id < m_durations.size())
    return m_durations[channel_id];
  else
    return {};
}

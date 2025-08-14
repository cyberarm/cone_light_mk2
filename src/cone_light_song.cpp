#include "cone_light_song.h"

String ConeLightSong::name() const
{
  return m_name;
}

std::vector<int8_t> ConeLightSong::channel_notes(uint8_t channel_id) const
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

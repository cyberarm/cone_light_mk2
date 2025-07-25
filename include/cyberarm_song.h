#pragma once

#include "cone_light_constants.h"

#define MAX_CHANNELS 8

//--- Cyberarm Song Channel
class CyberarmSongChannel
{
private:
  uint8_t m_pin = 0;
  std::vector<int16_t> m_notes = {};
  std::vector<uint16_t> m_durations = {};
  uint16_t m_current_note_id = 0;
  bool m_complete = true;
  uint32_t m_last_note_duration = 0;
  uint32_t m_last_note_started_at = 0;

public:
  CyberarmSongChannel() {}
  ~CyberarmSongChannel() {}

  void init(uint8_t pin, const std::vector<int16_t> notes, const std::vector<uint16_t> durations)
  {
    m_pin = pin;

    m_current_note_id = 0;
    m_complete = false;
    m_last_note_duration = 0;
    m_last_note_started_at = millis();

    m_notes = notes;
    m_durations = durations;

    Serial.printf("Initialized Channel on pin %d with %d notes\n", m_pin, m_notes.size());
  }

  void reset()
  {
    m_current_note_id = 0;
    m_complete = false;
    m_last_note_duration = 0;
    m_last_note_started_at = millis();
  }

  void update()
  {
    // Serial.printf("Channel#update: note#: %d, note count: %d\n", m_current_note_id, m_notes.size());

    if (millis() - m_last_note_started_at >= m_last_note_duration)
    {
      if (m_current_note_id >= m_notes.size())
      {
        ledcWriteTone(m_pin, 0);
        m_complete = true;
        return;
      }

      m_last_note_started_at = millis();
      int16_t note = m_notes.at(m_current_note_id);
      uint16_t duration = m_durations.at(m_current_note_id);
      m_last_note_duration = duration;

      if (note < 0)
      {
        ledcWriteTone(m_pin, 0);
      }
      else
      {
#if CONE_LIGHT_DEBUG_WITH_SILENT_SPEAKER
        ledcWriteTone(m_pin, 0);
#else
        ledcWriteTone(m_pin, note);
#endif
      }

#if CONE_LIGHT_DEBUG
      Serial.printf("PIN: %d, NOTE_ID: %d, FREQUENCY: %d, DURATION: %ld\n", m_pin, m_current_note_id, note, m_last_note_duration);
#endif

      m_current_note_id++;
    }
  }

  bool finished()
  {
    return m_complete;
  }

  uint16_t current_note_id()
  {
    return m_current_note_id;
  }

  uint16_t current_frequency()
  {
    if (m_current_note_id >= m_notes.size())
      return 0;

    return m_notes.at(m_current_note_id);
  }

  uint16_t current_duration()
  {
    if (m_current_note_id >= m_notes.size())
      return 0;

    return m_durations.at(m_current_note_id);
  }

  uint16_t next_note_id()
  {
    return m_current_note_id + 1;
  }

  uint16_t next_frequency()
  {
    if (next_note_id() >= m_notes.size())
      return 0;

    return m_notes.at(next_note_id());
  }

  uint16_t next_duration()
  {
    if (next_note_id() >= m_notes.size())
      return 0;

    return m_durations.at(next_note_id());
  }

  uint16_t note_count()
  {
    return m_notes.size();
  }

  const std::vector<int16_t> &notes()
  {
    return m_notes;
  }

  const std::vector<uint16_t> &durations()
  {
    return m_durations;
  }
};

//--- Cyberarm Song
class CyberarmSong
{
private:
  CyberarmSongChannel *m_channels[MAX_CHANNELS] = {};

public:
  CyberarmSong()
  {
    for (size_t i = 0; i < MAX_CHANNELS; i++)
    {
      m_channels[i] = new CyberarmSongChannel();
    }
  }
  ~CyberarmSong() {}
  void init() {}
  void update()
  {
    // Serial.printf("Song#update: channel 0 has %d left of %d notes\n", m_channels[0]->currentNote(), m_channels[0]->noteCount());

    for (size_t i = 0; i < MAX_CHANNELS; i++)
    {
      CyberarmSongChannel *channel = m_channels[i];

      if (!channel->finished())
      {
        channel->update();
      }
    }
  }

  bool playing()
  {
    for (size_t i = 0; i < MAX_CHANNELS; i++)
    {
      CyberarmSongChannel *channel = m_channels[i];

      if (!channel->finished())
        return true;
    }

    return false;
  }

  void reset()
  {
    for (size_t i = 0; i < MAX_CHANNELS; i++)
    {
      CyberarmSongChannel *channel = m_channels[i];

      channel->reset();
    }
  }

  void set_channel(uint8_t pin, uint8_t channelID, const std::vector<int16_t> &notes, const std::vector<uint16_t> &durations)
  {
    CyberarmSongChannel *channel = m_channels[channelID];

    channel->reset();
    channel->init(pin, notes, durations);
  }

  CyberarmSongChannel* channel(uint8_t channel_id)
  {
    if (channel_id >= MAX_CHANNELS)
      return nullptr;

    return m_channels[channel_id];
  }
};

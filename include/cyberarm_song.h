#pragma once

#include "cone_light_constants.h"
#include "cone_light_songs.h"

constexpr uint16_t note_to_freq[] = {
    8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15,
    16, 17, 18, 19, 21, 22, 23, 25, 26, 28, 29, 31,
    33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,
    65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123,
    131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247,
    262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
    523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
    1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
    2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
    4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
    8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

//--- Cyberarm Song Channel
class CyberarmSongChannel
{
private:
  uint8_t m_pin = 0;
  uint32_t m_song_data_index = 0;
  uint16_t m_current_note_id = 0;
  bool m_complete = true;
  uint32_t m_last_note_duration = 0;
  uint32_t m_last_note_started_at = 0;
  uint32_t m_song_real_note_count = 0;

  int8_t m_note = -1;
  uint16_t m_duration = 0;

public:
  CyberarmSongChannel() {}
  ~CyberarmSongChannel() {}

  void init(uint8_t pin, uint32_t song_data_index, int8_t note = -1, uint16_t duration = 0)
  {
    m_pin = pin;

    m_current_note_id = 0;
    m_complete = false;
    m_last_note_duration = 0;
    m_last_note_started_at = millis();

    m_song_data_index = song_data_index;
    m_note = note;
    m_duration = duration;

    if (note >= 0)
    {
      m_song_real_note_count = 1;
    }
    else
    {
      for (size_t i = 0; i < cone_light_song_notes[song_data_index].size(); i++)
      {
        auto note = cone_light_song_notes[song_data_index].at(i);

        if (note == CONE_LIGHT_SONG_END_NOTE)
          break;

        m_song_real_note_count = i + 1; // array INDEX to COUNT
      }
    }

    Serial.printf("Initialized Channel on pin %d with %d notes\n", m_pin, m_song_real_note_count);
  }

  void reset()
  {
    m_current_note_id = 0;
    m_complete = false;
    m_last_note_duration = 0;
    m_last_note_started_at = millis();
    m_song_real_note_count = 0;

    m_note = -1;
    m_duration = 0;

    ledcWriteTone(m_pin, 0);
  }

  void update()
  {
    // Serial.printf("Channel#update: note#: %d, note count: %d\n", m_current_note_id, m_notes.size());

    if (millis() - m_last_note_started_at >= m_last_note_duration)
    {
      if (m_current_note_id >= m_song_real_note_count)
      {
        ledcWriteTone(m_pin, 0);
        m_complete = true;
        return;
      }

      m_last_note_started_at = millis();
      int8_t note;
      uint16_t duration;
      if (m_note >= 0)
      {
        note = m_note;
        duration = m_duration;
      }
      else
      {
        note = cone_light_song_notes[m_song_data_index].at(m_current_note_id);
        duration = cone_light_song_durations[m_song_data_index].at(m_current_note_id);
      }
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
        ledcWriteTone(m_pin, note_to_freq[note]);
#endif
      }

#if CONE_LIGHT_DEBUG
      Serial.printf("PIN: %d, NOTE_ID: %d, FREQUENCY: %d, NOTE: %d, DURATION: %ld\n", m_pin, m_current_note_id, note_to_freq[note], note, m_last_note_duration);
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

  uint32_t song_data_index()
  {
    return m_song_data_index;
  }

  uint32_t song_real_note_count()
  {
    return m_song_real_note_count;
  }
};

//--- Cyberarm Song
class CyberarmSong
{
private:
  CyberarmSongChannel *m_channels[CONE_LIGHT_SONG_CHANNELS] = {};

public:
  CyberarmSong()
  {
    for (size_t i = 0; i < CONE_LIGHT_SONG_CHANNELS; i++)
    {
      m_channels[i] = new CyberarmSongChannel();
    }
  }
  ~CyberarmSong() {}
  void init() {}
  void update()
  {
    // Serial.printf("Song#update: channel 0 has %d left of %d notes\n", m_channels[0]->currentNote(), m_channels[0]->noteCount());

    for (size_t i = 0; i < CONE_LIGHT_SONG_CHANNELS; i++)
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
    for (size_t i = 0; i < CONE_LIGHT_SONG_CHANNELS; i++)
    {
      CyberarmSongChannel *channel = m_channels[i];

      if (!channel->finished())
        return true;
    }

    return false;
  }

  void reset()
  {
    for (size_t i = 0; i < CONE_LIGHT_SONG_CHANNELS; i++)
    {
      CyberarmSongChannel *channel = m_channels[i];

      channel->reset();
    }
  }

  void set_channel(uint8_t pin, uint8_t channelID, uint32_t song_data_index, int8_t note = -1, uint16_t duration = 0)
  {
    CyberarmSongChannel *channel = m_channels[channelID];

    channel->reset();
    channel->init(pin, song_data_index, note, duration);
  }

  CyberarmSongChannel *channel(uint8_t channel_id)
  {
    if (channel_id >= CONE_LIGHT_SONG_CHANNELS)
      return nullptr;

    return m_channels[channel_id];
  }
};

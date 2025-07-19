#pragma once

#define MAX_NOTES 2048
#define MAX_CHANNELS 8

//--- Cyberarm Song Channel
class CyberarmSongChannel
{
private:
  uint8_t m_pin = 0;
  int16_t m_notes[MAX_NOTES] = {0};
  uint16_t m_durations[MAX_NOTES] = {0};
  uint16_t m_note_count = 0;
  uint16_t m_current_note = 0;
  bool m_complete = true;
  uint32_t m_last_note_duration = 0;
  uint32_t m_last_note_started_at = 0;

public:
  CyberarmSongChannel() {}
  ~CyberarmSongChannel() {}

  void init(uint8_t pin, uint16_t noteCount, int16_t notes[], uint16_t durations[])
  {
    m_pin = pin;
    m_note_count = noteCount;

    m_current_note = 0;
    m_complete = false;
    m_last_note_duration = 0;
    m_last_note_started_at = millis();

    for (size_t i = 0; i < MAX_NOTES; i++)
    {
      if (i < m_note_count)
      {
        m_notes[i] = notes[i];
        m_durations[i] = durations[i];
      }
      else
      {
        m_notes[i] = 0;
        m_durations[i] = 0;
      }
    }

    Serial.printf("Initialized Channel on pin %d with %d notes\n", m_pin, m_note_count);
  }

  void reset()
  {
    m_current_note = 0;
    m_complete = false;
    m_last_note_duration = 0;
    m_last_note_started_at = millis();
  }

  void update()
  {
    // Serial.printf("Channel#update: note#: %d, note count: %d\n", m_current_note, m_note_count);

    if (millis() - m_last_note_started_at >= m_last_note_duration)
    {
      if (m_current_note >= m_note_count)
      {
        ledcWriteTone(m_pin, 0);
        m_complete = true;
        return;
      }

      m_last_note_started_at = millis();
      int16_t note = m_notes[m_current_note];
      uint16_t duration = m_durations[m_current_note];
      m_last_note_duration = duration;

      if (note < 0)
      {
        ledcWriteTone(m_pin, 0);
      }
      else
      {
        ledcWriteTone(m_pin, note);
      }

#if CONE_LIGHT_DEBUG
      Serial.printf("PIN: %d, NOTE_ID: %d, FREQUENCY: %d, DURATION: %ld\n", m_pin, m_current_note, note, m_last_note_duration);
#endif

      m_current_note++;
    }
  }

  bool isFinished()
  {
    return m_complete;
  }

  uint16_t currentNote()
  {
    return m_current_note;
  }

  uint16_t currentFrequency()
  {
    if (m_current_note >= m_note_count)
      return 0;

    return m_notes[m_current_note];
  }

  uint16_t noteCount()
  {
    return m_note_count;
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

      if (!channel->isFinished())
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

      if (!channel->isFinished())
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

  void set_channel(uint8_t pin, uint8_t channelID, uint16_t noteCount, int16_t notes[], uint16_t durations[])
  {
    CyberarmSongChannel *channel = m_channels[channelID];

    channel->reset();
    channel->init(pin, noteCount, notes, durations);
  }

  CyberarmSongChannel* channel(uint8_t channel_id)
  {
    if (channel_id >= MAX_CHANNELS)
      return nullptr;

    return m_channels[channel_id];
  }
};

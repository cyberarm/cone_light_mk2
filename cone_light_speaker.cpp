#include "cone_light_songs.h"
#include "cone_light_speaker.h"

ConeLightSpeaker::ConeLightSpeaker(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  Serial.printf("    Configuring Speaker: %d\n", ledcAttach(SPEAKER, 8000, 10));

  const int8_t note_count = 7;
  int16_t notes[note_count] = {136, -1, 220, -1, 293, -1, 370};
  uint16_t durations[note_count] = {667, 32, 667, 32, 667, 32, 667};

  // Boot chime
  if (CONE_LIGHT_BOOT_UP_TUNE)
  {
    m_song->channel(SPEAKER, 0, note_count, notes, durations);
  }
  else
  {
    reset();
  }

  Serial.println("    Speaker initialized successfully.");
}

ConeLightSpeaker::~ConeLightSpeaker()
{
}

void ConeLightSpeaker::update()
{
  m_song->update();
}

void ConeLightSpeaker::reset()
{
  m_song->reset();
}

void ConeLightSpeaker::play_song(uint16_t song_id)
{
  ConeLightSong *song = cone_light_songs[song_id];
  if (!song)
    return;

  m_song->reset();

  std::vector<int16_t> notes = song->channel_notes(m_cone_light->node_id());
  std::vector<uint16_t> durations = song->channel_durations(m_cone_light->node_id());

  m_song->channel(SPEAKER, m_cone_light->node_id(), notes.size(), notes.data(), durations.data());
}

void ConeLightSpeaker::play_tone(uint16_t frequency, uint16_t duration)
{
  m_song->reset();

  int16_t notes[1] = {0};
  uint16_t durations[1] = {0};

  notes[0] = frequency;
  durations[0] = duration;

  m_song->channel(SPEAKER, 0, 1, notes, durations);
}

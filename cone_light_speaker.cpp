#include "cone_light_songs.h"
#include "cone_light_speaker.h"

ConeLightSpeaker::ConeLightSpeaker(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  m_speaker_inited = ledcAttach(SPEAKER, 8000, 10);

  const int8_t note_count = 7;
  int16_t notes[note_count] = {136, -1, 220, -1, 293, -1, 370};
  uint16_t durations[note_count] = {667, 32, 667, 32, 667, 32, 667};

  // Boot chime
  if (m_speaker_inited && CONE_LIGHT_BOOT_UP_TUNE)
  {
    m_song->channel(SPEAKER, 0, note_count, notes, durations);
  }
  else
  {
    reset();
  }

  if (m_speaker_inited)
    Serial.println("    Speaker initialized successfully.");
  else
    Serial.println("    Speaker failed to initialize.");
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
  m_song->reset();

  if (!m_speaker_inited)
  {
    Serial.println("    Speaker failed to initialize, not attempting to play song.");
    return;
  }

  // song_id is an array index
  if (song_id >= cone_light_songs.size())
  {
    printf("    Speaker Song: Unknown song id: %d\n", song_id);
    return;
  }

  ConeLightSong song = cone_light_songs[song_id];

  uint8_t node_id = m_cone_light->node_id();
  if (node_id >= MAX_CHANNELS)
  {
    printf("    Speaker Song: Node ID [%d] greater than available song channels [%d - 1]\n    Misconfigured?", node_id, MAX_CHANNELS);
    return;
  }

  std::vector<int16_t> notes = song.channel_notes(node_id);
  std::vector<uint16_t> durations = song.channel_durations(node_id);

  printf("    Speaker Song: %s, Channel [%d] Notes: %d\n", song.name().c_str(), node_id, notes.size());

  m_song->channel(SPEAKER, node_id, notes.size(), notes.data(), durations.data());
}

void ConeLightSpeaker::play_tone(uint16_t frequency, uint16_t duration)
{
  m_song->reset();

  if (!m_speaker_inited)
  {
    Serial.println("    Speaker failed to initialize, not attempting to play tone.");
    return;
  }

  int16_t notes[1] = {0};
  uint16_t durations[1] = {0};

  notes[0] = frequency;
  durations[0] = duration;

  m_song->channel(SPEAKER, 0, 1, notes, durations);
}

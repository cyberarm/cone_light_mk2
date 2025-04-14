#include "cone_light_speaker.h"

ConeLightSpeaker::ConeLightSpeaker(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  Serial.printf("SPEAKER %d\n", ledcAttach(SPEAKER, 32000, 10));

  const int8_t note_count = 7;
  int16_t notes[note_count] = {136, -1, 220, -1, 293, -1, 370};
  uint16_t durations[note_count] = {667, 32, 667, 32, 667, 32, 667};

  // Boot chime
  if (CONE_LIGHT_BOOT_UP_TUNE)
  {
    m_song->channel(SPEAKER, 0, note_count, notes, durations);
  } else
  {
    reset();
  }
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

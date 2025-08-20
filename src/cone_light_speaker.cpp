#include "cone_light_songs.h"
#include "cone_light_speaker.h"

ConeLightSpeaker::ConeLightSpeaker(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  m_speaker_inited = ledcAttach(SPEAKER_PIN, 8000, 10);

  m_notes = {48, -1, 56, -1, 63, -1, 67};
  m_durations = {667, 32, 667, 32, 667, 32, 667};

  // Boot chime
  // if (m_speaker_inited && CONE_LIGHT_BOOT_UP_TUNE)
  // {
  //   m_song->set_channel(SPEAKER_PIN, m_cone_light->node_id(), m_notes, m_durations);
  // }
  // else
  // {
    reset();
  // }

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

  // HARM: Potentially rapidly changing/flashing lights
  if (m_song_playing)
  {
    m_led_timeline.update();

    // FIXME: Continually updating the leds might prevent the wifi task from runnning, preventing sending/receiving packets.
    // FIXME: Allow enabling/disabling LED animation from node's instead of hardcoding on or off
#if CONE_LIGHT_ALLOW_PHOTOSENSITIVE_HAZARDS
    m_cone_light->lighting()->set_color(m_led_song_color);
    m_cone_light->lighting()->set_brightness(m_cone_light->lighting()->get_static_brightness());
#endif

    if (!m_song->playing() && !m_led_timeline.isRunning())
    {
      m_song_playing = false;
      m_cone_light->lighting()->set_color(m_cone_light->lighting()->get_static_color());
      m_cone_light->lighting()->set_brightness(m_cone_light->lighting()->get_static_brightness());
    }
  }
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
  if (song_id >= cone_light_song_titles.size())
  {
    printf("    Speaker Song: Unknown song id: %d\n", song_id);
    return;
  }

  uint8_t node_id = m_cone_light->node_id();
  if (node_id >= CONE_LIGHT_SONG_CHANNELS)
  {
    printf("    Speaker Song: Node ID [%d] greater than available song channels [%d - 1]\n    Misconfigured?", node_id, CONE_LIGHT_SONG_CHANNELS);
    return;
  }

  printf("    Speaker Song: %s, Channel [%d] Notes: %d\n", cone_light_song_titles[song_id].c_str(), node_id, cone_light_song_notes[(song_id * CONE_LIGHT_SONG_CHANNELS) + m_cone_light->node_id()].size());

  m_song->set_channel(SPEAKER_PIN, node_id, (song_id * CONE_LIGHT_SONG_CHANNELS) + m_cone_light->node_id());
  animate_leds_with_song();
  m_song_playing = true;
}

void ConeLightSpeaker::play_tone(uint8_t note, uint16_t duration)
{
  // m_song->reset();

  // if (!m_speaker_inited)
  // {
  //   Serial.println("    Speaker failed to initialize, not attempting to play tone.");
  //   return;
  // }

  // m_notes = {(int8_t)note, CONE_LIGHT_SONG_END_NOTE};
  // m_durations = {duration, 0};

  // m_song->set_channel(SPEAKER_PIN, m_cone_light->node_id(), m_notes, m_durations);
}

void ConeLightSpeaker::handle_packet(cone_light_network_packet_t packet)
{
  uint8_t node_group_id = packet.command_parameters_extra;

  // Ignore packets not mean for this node
  // node group 255 is unset/all groups
  if (node_group_id != 255 && m_cone_light->node_group_id() != node_group_id)
    return;

  // TODO: Introduce a time delay?

  switch (packet.command_type)
  {
  case PLAY_SONG:
    play_song(packet.command_parameters);
    break;

  case PLAY_TONE:
  {
    uint16_t frequency = (packet.command_parameters >> 16 & 0xFFFF);
    uint16_t duration = (packet.command_parameters >> 0 & 0xFFFF);

    play_tone(frequency, duration);
    break;
  }

  default:
    break;
  }
}

void ConeLightSpeaker::animate_leds_with_song()
{
  CyberarmSongChannel *channel = m_song->channel(m_cone_light->node_id());
  m_led_timeline.clear();
  m_led_song_color = CRGB::Black;

  m_led_timeline.add(m_led_song_color);
  uint16_t skipped_rest_duration_ms = 0;

  if (channel && channel->song_real_note_count() > 0)
  {
    for (size_t i = 0; i < channel->song_real_note_count(); i++)
    {
      auto note = cone_light_song_notes[channel->song_data_index()].at(i);
      auto duration = cone_light_song_durations[channel->song_data_index()].at(i);
      auto color = m_cone_light->lighting()->note_to_color(note);

      // First note, check for leading pause
      if (i == 0 && note < 0)
      {
        m_led_timeline.append(m_led_song_color, CRGB::Black, duration);
      }
      else
      {
        // Don't animate short rests to mitigate flashing
        if (note < 0 && duration < 128)
        {
          skipped_rest_duration_ms += duration;
          continue;
        }

        m_led_timeline.append(m_led_song_color, color, duration + skipped_rest_duration_ms);
        skipped_rest_duration_ms = 0;
      }
    }
  }

  // Fade out LEDs at end of song
  m_led_timeline.append(m_led_song_color, CRGB::Black, 500);
  // Hold on black for a moment
  m_led_timeline.append(m_led_song_color, CRGB::Black, 500);
  // Reset LED color back to what it was before the song began
  m_led_timeline.append(m_led_song_color, m_cone_light->lighting()->get_static_color(), 500);

  m_led_timeline.mode(Tween::Mode::ONCE);
  m_led_timeline.start();
}

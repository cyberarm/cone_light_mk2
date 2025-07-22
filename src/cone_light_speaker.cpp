#include "cone_light_songs.h"
#include "cone_light_speaker.h"

ConeLightSpeaker::ConeLightSpeaker(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  m_speaker_inited = ledcAttach(SPEAKER_PIN, 8000, 10);

  const int8_t note_count = 7;
  int16_t notes[note_count] = {136, -1, 220, -1, 293, -1, 370};
  uint16_t durations[note_count] = {667, 32, 667, 32, 667, 32, 667};

  // Boot chime
  if (m_speaker_inited && CONE_LIGHT_BOOT_UP_TUNE)
  {
    m_song->set_channel(SPEAKER_PIN, 0, note_count, notes, durations);
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

  // HARM: Potentially rapidly changing/flashing lights
  // TODO: Make leds 'blend' between current and next colors to prevent/mitigate flashing
  if (m_song_playing)
  {
    m_led_timeline.update();

    // FIXME: Continually updating the leds might prevent the wifi task from runnning, preventing sending/receiving packets.
#if CONE_LIGHT_ALLOW_PHOTOSENSITIVE_HAZARDS
    m_cone_light->lighting()->set_color(m_led_song_color);
    m_cone_light->lighting()->set_brightness(LED_DEFAULT_BRIGHTNESS);
#endif

    if (!m_song->playing() && m_led_timeline.getRemainingTime() <= 0)
      m_song_playing = false;
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

  m_song->set_channel(SPEAKER_PIN, node_id, notes.size(), notes.data(), durations.data());
  animate_leds_with_song();
  m_song_playing = true;
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

  m_song->set_channel(SPEAKER_PIN, 0, 1, notes, durations);
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

  if (channel && channel->note_count() > 0)
  {
    for (size_t i = 0; i < channel->note_count() - 1; i++)
    {
      auto freq = channel->notes()[i];
      auto duration = channel->durations()[i];
      auto color = m_cone_light->lighting()->frequency_to_color(freq);

      // First note, check for leading pause
      if (i == 0 && freq < 0)
      {
        m_led_timeline.append(m_led_song_color, CRGB::Black, duration);
      }
      else
      {
        // Don't animate short rests to mitigate flashing
        if (freq < 0 && duration < 128)
        {
          skipped_rest_duration_ms += duration;
          continue;
        }

        m_led_timeline.append(m_led_song_color, color, duration + skipped_rest_duration_ms);
        skipped_rest_duration_ms = 0;
      }
    }
  }

  // Reset LED color back to what it was before the song began
  // FIXME: Store value of SET_COLOR/SET_GROUP_COLOR commands to ensure we use the correct color
  //        and don't accidentally use a color from a mid progress song that's interrupted.
  m_led_timeline.append(m_led_song_color, m_cone_light->lighting()->get_color(), 500);

  m_led_timeline.mode(Tween::Mode::ONCE);
  m_led_timeline.start();
}

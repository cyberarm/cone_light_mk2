#include "include/cone_light_network_packets.h"

// forward declarations :'(
class ConeLight;
typedef struct cone_light_network_packet cone_light_network_packet_t;

extern ConeLight *g_cone_light;


cone_light_network_packet_t
cone_light_packet_custom(uint8_t command_type, uint32_t parameters,
                         uint32_t parameters_extra) {
  cone_light_network_packet_t packet = {};

  packet.command_id = g_cone_light->networking()->next_command_id();
  packet.command_type = command_type;
  packet.command_parameters = parameters;
  packet.command_parameters_extra = parameters_extra;

  return packet;
}

//--- PING / PONG ---//
cone_light_network_packet_t cone_light_packet_ping() {
  return cone_light_packet_custom(ConeLightNetworkCommand::PING, 0, 0);
}

cone_light_network_packet_t
cone_light_packet_pong(float voltage, bool is_remote, bool is_playing) {
  uint32_t packed_voltage;
  memcpy(&packed_voltage, &voltage, sizeof(float));

  // Pack some booleans into a uint32_t (unsigned long)
  std::bitset<32> bits{0};
  if (is_remote)
    bits.set(ConeLightPongBit::IS_REMOTE);
  if (is_playing)
    bits.set(ConeLightPongBit::IS_PLAYING);

  return cone_light_packet_custom(ConeLightNetworkCommand::PONG, packed_voltage,
                                  bits.to_ulong());
}

//--- NETWORK TIME ---//
cone_light_network_packet_t cone_light_packet_clock_grandmaster_sync() {
  return cone_light_packet_custom(
      ConeLightNetworkCommand::CLOCK_GRANDMASTER_SYNC, 0, 0);
}

cone_light_network_packet_t
cone_light_packet_clock_node_delay_request(uint32_t delay_request_time_ms) {
  return cone_light_packet_custom(
      ConeLightNetworkCommand::CLOCK_NODE_DELAY_REQUEST,
      delay_request_time_ms, 0);
}

cone_light_network_packet_t
cone_light_packet_clock_node_delay_response(uint32_t timestamp,
                                            uint8_t target_node_id) {
  return cone_light_packet_custom(
      ConeLightNetworkCommand::CLOCK_NODE_DELAY_RESPONSE, timestamp,
      target_node_id);
}

//--- LED CONTROL ---//
cone_light_network_packet_t
cone_light_packet_set_color(uint8_t red, uint8_t green, uint8_t blue,
                            uint8_t brightness, uint8_t node_or_group_id) {
  uint32_t packed_color = uint32_t{brightness} << 24 | (uint32_t{red} << 16) |
                          (uint32_t{green} << 8) | (uint32_t{blue});

  return cone_light_packet_custom(ConeLightNetworkCommand::SET_COLOR,
                                  packed_color, node_or_group_id);
}

cone_light_network_packet_t
cone_light_packet_set_group_color(uint8_t red, uint8_t green, uint8_t blue,
                                  uint8_t brightness,
                                  uint8_t node_or_group_id) {
  uint32_t packed_color = uint32_t{brightness} << 24 | (uint32_t{red} << 16) |
                          (uint32_t{green} << 8) | (uint32_t{blue});

  return cone_light_packet_custom(ConeLightNetworkCommand::SET_GROUP_COLOR,
                                  packed_color, node_or_group_id);
}

cone_light_network_packet_t
cone_light_packet_set_brightness(uint8_t brightness, uint8_t node_or_group_id) {
  return cone_light_packet_custom(ConeLightNetworkCommand::SET_BRIGHTNESS,
                                  brightness, node_or_group_id);
}

cone_light_network_packet_t
cone_light_packet_set_group_brightness(uint8_t brightness,
                                       uint8_t node_or_group_id) {
  return cone_light_packet_custom(ConeLightNetworkCommand::SET_GROUP_BRIGHTNESS,
                                  brightness, node_or_group_id);
}

//--- TONE / SONG CONTROL ---//
cone_light_network_packet_t
cone_light_packet_play_tone(uint8_t note, uint16_t duration,
                            uint8_t node_or_group_id, bool is_group) {
  uint32_t packed_parameters =
      uint32_t{node_or_group_id} << 8 | (uint32_t{0} << 0);

  uint32_t packed_note_and_duration = uint32_t{note} << 16 | uint32_t{duration};

  return cone_light_packet_custom(is_group
                                      ? ConeLightNetworkCommand::PLAY_GROUP_TONE
                                      : ConeLightNetworkCommand::PLAY_TONE,
                                  packed_note_and_duration, packed_parameters);
}

cone_light_network_packet_t
cone_light_packet_play_group_tone(uint8_t note, uint16_t duration,
                                  uint8_t node_or_group_id) {
  return cone_light_packet_play_tone(note, duration, node_or_group_id, true);
}

cone_light_network_packet_t
cone_light_packet_play_song(uint32_t song_id, int8_t transpose,
                            uint8_t node_or_group_id, bool is_group) {
  uint32_t packed_parameters =
      uint32_t{node_or_group_id} << 8 | (uint32_t{transpose} << 0);

  return cone_light_packet_custom(is_group
                                      ? ConeLightNetworkCommand::PLAY_GROUP_SONG
                                      : ConeLightNetworkCommand::PLAY_SONG,
                                  song_id, packed_parameters);
}

cone_light_network_packet_t
cone_light_packet_play_group_song(uint32_t song_id, int8_t transpose,
                                  uint8_t node_or_group_id) {
  return cone_light_packet_play_tone(song_id, transpose, node_or_group_id,
                                     true);
}

//--- NOT IMPLEMENTED ---//
cone_light_network_packet_t
cone_light_packet_start_animation(uint8_t command_type, uint32_t parameters,
                                  uint32_t parameters_extra) {}

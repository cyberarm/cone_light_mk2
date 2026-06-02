#pragma once
#include <cstdint>
#include <bitset>

#include "cone_light.h"
#include "cone_light_constants.h"
#include "cone_light_network_packet.h"

cone_light_network_packet_t cone_light_packet_custom(uint8_t command_type, uint32_t parameters, uint32_t parameters_extra);

cone_light_network_packet_t cone_light_packet_ping();
cone_light_network_packet_t cone_light_packet_pong(float voltage, bool is_remote, bool is_playing);

cone_light_network_packet_t cone_light_packet_clock_grandmaster_sync();
cone_light_network_packet_t cone_light_packet_clock_node_delay_request(uint32_t delay_request_time_ms);
cone_light_network_packet_t cone_light_packet_clock_node_delay_response(uint32_t timestamp, uint8_t target_node_id);

cone_light_network_packet_t cone_light_packet_set_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness, uint8_t node_or_group_id, bool is_group = false);
cone_light_network_packet_t cone_light_packet_set_color(uint32_t packed_color, uint8_t node_or_group_id, bool is_group = false);
cone_light_network_packet_t cone_light_packet_set_group_color(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness, uint8_t node_or_group_id);
cone_light_network_packet_t cone_light_packet_set_brightness(uint8_t brightness, uint8_t node_or_group_id, bool is_group = false);
cone_light_network_packet_t cone_light_packet_set_group_brightness(uint8_t brightness, uint8_t node_or_group_id);

cone_light_network_packet_t cone_light_packet_play_tone(uint8_t note, uint16_t duration, uint8_t node_or_group_id, bool is_group = false);
cone_light_network_packet_t cone_light_packet_play_group_tone(uint8_t note, uint16_t duration, uint8_t node_or_group_id);
cone_light_network_packet_t cone_light_packet_play_song(uint32_t song_id, int8_t transpose, uint8_t node_or_group_id, bool is_group = false);
cone_light_network_packet_t cone_light_packet_play_group_song(uint32_t song_id, int8_t transpose, uint8_t node_or_group_id);

cone_light_network_packet_t cone_light_packet_start_animation(uint8_t command_type, uint32_t parameters, uint32_t parameters_extra);

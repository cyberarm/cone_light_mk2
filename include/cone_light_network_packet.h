#pragma once
#include <cstdint>

typedef struct cone_light_network_packet
{
  uint8_t firmware_version;
  uint16_t packet_id;
  uint32_t timestamp;
  uint8_t node_id;
  char node_name[6];
  uint8_t node_group_id;
  uint16_t command_id;
  uint8_t command_type;
  uint32_t command_parameters;
} cone_light_network_packet_t;

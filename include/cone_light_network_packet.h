#pragma once
#include <cstdint>

typedef struct cone_light_network_packet
{
  // Automatically filled by Networking#send_packet()
  char protocol_id[5];
  uint8_t firmware_version;
  uint32_t packet_id;
  uint32_t timestamp; // (unsynced) local time of node
  uint8_t node_id;
  char node_name[7];
  uint8_t node_group_id;
  // Bits that do stuff down here
  uint32_t command_id;
  uint8_t command_type;
  uint32_t command_parameters;
  uint32_t command_parameters_extra;
} cone_light_network_packet_t;

#pragma once
#include <Arduino.h>

typedef struct cone_light_network_packet
{
  uint16_t id;
  uint8_t peer_id;
} cone_light_network_packet_t;

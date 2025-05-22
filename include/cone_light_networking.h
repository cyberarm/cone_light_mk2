#pragma once

#include "cone_light.h"
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

// Forward declaration...
class ConeLight;

typedef struct cone_light_network_packet
{
  uint8_t peer_id;
} cone_light_network_packet_t;

class ConeLightNetworking
{
private:
  ConeLight *m_cone_light = nullptr;
  uint8_t m_broadcast_address[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

public:
  ConeLightNetworking(ConeLight *cone_light);
  ~ConeLightNetworking();
  void update();
  void send_packet(const uint8_t *mac_addr, cone_light_network_packet_t packet);
  void broadcast_packet(cone_light_network_packet_t packet);
  void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void on_data_received(const uint8_t *mac_addr, const uint8_t *data, int len);
};

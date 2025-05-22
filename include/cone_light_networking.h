#pragma once

#include "cone_light.h"
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

// Forward declaration...
class ConeLight;

class ConeLightNetworking
{
private:
  ConeLight *m_cone_light = nullptr;

public:
  ConeLightNetworking(ConeLight *cone_light);
  ~ConeLightNetworking();
  void update();
  void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void on_data_received(const uint8_t *mac_addr, const uint8_t *incomingData, int len);
};

typedef struct cone_light_network_packet
{
  uint8_t peer_id;
} cone_light_network_packet_t;

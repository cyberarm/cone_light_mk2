#pragma once

#include "cone_light.h"
#include <esp_now.h>
#include <WiFi.h>
#include <functional>

// Forward declaration...
class ConeLight;

typedef struct cone_light_network_packet
{
  uint16_t id;
  uint8_t peer_id;
} cone_light_network_packet_t;

class ConeLightNetworking
{
private:
  ConeLight *m_cone_light = nullptr;
  uint8_t m_broadcast_address[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  bool m_espnow_initialized = false;

public:
  ConeLightNetworking(ConeLight *cone_light);
  ~ConeLightNetworking();
  void update();
  bool espnow_initialized() { return m_espnow_initialized; };
  void send_packet(const uint8_t *mac_addr, cone_light_network_packet_t packet);
  void broadcast_packet(cone_light_network_packet_t packet);
  void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void on_data_received(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len);
};

void cone_light_networking_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status);
void cone_light_networking_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len);

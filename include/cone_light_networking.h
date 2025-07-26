#pragma once

#include "cone_light_network_packet.h"
#include "cone_light.h"
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <functional>

// Forward declaration...
class ConeLight;

typedef struct cone_light_networking_node_tracker
{
  bool m_first_packet = true;
  int8_t m_address[6] = {0};
  int16_t m_rssi = 0;

  uint32_t timestamp = 0;
  uint8_t node_id = 255;
  uint8_t node_group_id = 255;
  char node_name[7] = "";
  uint32_t packet_id = 0;
  uint32_t command_id = 0;

  bool ingest_packet(const esp_now_recv_info_t *esp_now_info, const cone_light_network_packet_t packet)
  {
    bool valid_packet_node_details = packet.node_id == node_id &&
                                     packet.node_group_id == node_group_id &&
                                     strncmp(packet.node_name, node_name, 7) == 0;

    bool valid_packet_ids = packet.packet_id > packet_id &&
                            packet.command_id > command_id;

    bool valid_packet = valid_packet_node_details && valid_packet_ids;

#if CONE_LIGHT_DEBUG
    Serial.printf("INGEST_PACKET: node_id: %u / %u, node_group_id: %u / %u, node_name: %s / %s [%u], packet_id: %u / %u, command_id: %d / %u\n",
                  node_id, packet.node_id, node_group_id, packet.node_group_id, node_name, packet.node_name, strncmp(packet.node_name, node_name, 7), packet_id, packet.packet_id, command_id, packet.command_id);
#endif

    // Short circuit since we have no data to disprove the packet's validity
    if (m_first_packet)
    {
      m_first_packet = false;
      valid_packet = true;

      // Set fixed node data
      memcpy(m_address, esp_now_info->src_addr, 6);

      timestamp = packet.timestamp;
      node_id = packet.node_id;
      node_group_id = packet.node_group_id;
      strncpy(node_name, packet.node_name, 7);
    }

    if (!valid_packet)
    {
      // Allow ignoring validity of packet/command id from node when node's timestamp is less than before (assume rebooted)
      if (!(valid_packet_node_details && !valid_packet_ids && packet.timestamp < timestamp))
        return false;

#if CONE_LIGHT_DEBUG
      Serial.printf("INGEST_PACKET: packet with valid node details, younger timestamp, and invalid packet/command IDs was accepted. Assumed that node rebooted.\n");
#endif
    }

    // Update node data
    m_rssi = esp_now_info->rx_ctrl->rssi;

    packet_id = packet.packet_id;
    command_id = packet.command_id;

    return true;
  }

  const int8_t *address() { return m_address; }
  const int16_t rssi() { return m_rssi; }

} cone_light_networking_node_tracker_t;

class ConeLightNetworkingRedundantPacketDelivery
{
private:
  uint8_t m_receipt_address[6];
  cone_light_network_packet_t m_packet;
  uint8_t m_redundant_deliveries = 2;
  uint8_t m_ms_between_deliveries = 5;
  uint32_t m_last_delivery_ms = 0;

public:
  ConeLightNetworkingRedundantPacketDelivery(const uint8_t *receipt_address, const cone_light_network_packet_t packet, const uint8_t redundant_deliveries, const uint8_t ms_between_deliveries, const uint32_t last_delivery_ms)
  {
    memcpy(m_receipt_address, receipt_address, 6);
    memcpy(&m_packet, &packet, sizeof(cone_light_network_packet_t));
    m_redundant_deliveries = redundant_deliveries;
    m_ms_between_deliveries = ms_between_deliveries;
    m_last_delivery_ms = last_delivery_ms;
  }

  ~ConeLightNetworkingRedundantPacketDelivery() {}

  const uint8_t *receipt_address() { return m_receipt_address; };
  const cone_light_network_packet_t packet() { return m_packet; };
  const uint8_t redundant_deliveries() { return m_redundant_deliveries; };
  const uint8_t ms_between_deliveries() { return m_ms_between_deliveries; };
  const uint32_t last_delivery_ms() { return m_last_delivery_ms; };

  void delivery_attempted()
  {
    m_redundant_deliveries--;
    if (m_redundant_deliveries >= 250)
      m_redundant_deliveries = 0;

    m_last_delivery_ms = millis();
  };
};

class ConeLightNetworking
{
private:
  ConeLight *m_cone_light = nullptr;
  uint8_t m_broadcast_address[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  bool m_espnow_initialized = false;
  uint32_t m_packet_id = 1;
  uint32_t m_command_id = 1;
  std::array<cone_light_networking_node_tracker_t, CONE_LIGHT_NETWORKING_MAX_NODES> m_known_nodes = {};
  const int8_t m_no_node_address[6] = {0};
  std::vector<std::shared_ptr<ConeLightNetworkingRedundantPacketDelivery>> m_redundant_packet_deliveries = {};

public:
  ConeLightNetworking(ConeLight *cone_light);
  ~ConeLightNetworking();
  void update();
  bool espnow_initialized() { return m_espnow_initialized; };
  void send_packet(const uint8_t *mac_addr, cone_light_network_packet_t packet, bool redundant_delivery = false);
  void broadcast_packet(cone_light_network_packet_t packet, bool redundant_delivery = false);
  void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void on_data_received(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len);
  uint32_t next_command_id() { return m_command_id++; };
  const int16_t node_rssi(uint8_t node_id)
  {
    if (node_id >= CONE_LIGHT_NETWORKING_MAX_NODES)
      return -32'768;

    return m_known_nodes[node_id].rssi();
  };
  const int8_t *node_address(uint8_t node_id)
  {
    if (node_id >= CONE_LIGHT_NETWORKING_MAX_NODES)
    {
      return m_no_node_address;
    }

    return m_known_nodes[node_id].address();
  };
};

void cone_light_networking_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status);
void cone_light_networking_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len);

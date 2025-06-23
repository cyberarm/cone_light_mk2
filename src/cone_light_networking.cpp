#include "cone_light_networking.h"

ConeLightNetworking::ConeLightNetworking(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  WiFi.mode(WIFI_MODE_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set Long Range mode
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

  esp_now_register_send_cb(cone_light_networking_send_callback);
  esp_now_register_recv_cb(cone_light_networking_recv_callback);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, m_broadcast_address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add broadcast peer!");

    return;
  }

  Serial.printf("    Networking initialized successfully. (MAC ADDR: %s)\n", WiFi.macAddress().c_str());
}

void ConeLightNetworking::update()
{
}

void ConeLightNetworking::send_packet(const uint8_t *mac_addr, cone_light_network_packet_t packet)
{
  const char *node_name = m_cone_light->node_name().c_str();

  packet.firmware_version = CONE_LIGHT_FIRMWARE_VERSION;
  packet.packet_id = m_packet_id++;
  packet.node_id = m_cone_light->node_id();
  for (size_t i = 0; i < 6; i++)
  {
    packet.node_name[i] = node_name[i];
  }
  packet.node_group_id = m_cone_light->node_group_id();

  esp_err_t result = esp_now_send(mac_addr, (uint8_t *)&packet, sizeof(packet));

  if (result != ESP_OK)
    Serial.printf("ESPNow failed to send packet: [%d] %s\n", result, esp_err_to_name(result));
}

void ConeLightNetworking::broadcast_packet(cone_light_network_packet_t packet)
{
  send_packet(m_broadcast_address, packet);
}

void ConeLightNetworking::on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
}

void ConeLightNetworking::on_data_received(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len)
{
  cone_light_network_packet_t packet;
  memcpy(&packet, data, sizeof(packet));

  m_cone_light->espnow_event(packet);
}

//--- non-member functions ---//
extern ConeLight *g_cone_light;
void cone_light_networking_send_callback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  g_cone_light->networking()->on_data_sent(mac_addr, status);
}

void cone_light_networking_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len)
{
  g_cone_light->networking()->on_data_received(esp_now_info, data, len);
}

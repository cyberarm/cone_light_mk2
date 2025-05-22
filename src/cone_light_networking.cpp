#include "cone_light_networking.h"

ConeLightNetworking::ConeLightNetworking(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  WiFi.mode(WIFI_MODE_STA);

  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // esp_now_register_send_cb(on_data_sent);
  // esp_now_register_recv_cb(on_data_received);
}

void ConeLightNetworking::update()
{
}

void ConeLightNetworking::send_packet(const uint8_t *mac_addr, cone_light_network_packet_t packet)
{
  esp_now_send(mac_addr, (uint8_t *)&packet, sizeof(packet));
}

void ConeLightNetworking::broadcast_packet(cone_light_network_packet_t packet)
{
  esp_now_send(m_broadcast_address, (uint8_t *)&packet, sizeof(packet));
}

void ConeLightNetworking::on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
}

void ConeLightNetworking::on_data_received(const uint8_t *mac_addr, const uint8_t *data, int len)
{
  cone_light_network_packet_t packet;
  memcpy(&packet, data, sizeof(packet));

  // m_cone_light->networking_event(packet);
}

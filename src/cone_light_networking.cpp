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

void ConeLightNetworking::on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
}

void ConeLightNetworking::on_data_received(const uint8_t *mac_addr, const uint8_t *incomingData, int len)
{
}

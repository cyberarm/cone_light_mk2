#include "cone_light_networking.h"

ConeLightNetworking::ConeLightNetworking(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  // See: https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/
  pinMode(WIFI_ENABLE, OUTPUT);   // pinMode(3, OUTPUT);
  digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control
  delay(100);
  pinMode(WIFI_ANT_CONFIG, OUTPUT);    // pinMode(14, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH); // digitalWrite(14, HIGH); // Use external antenna

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
  // uint32_t ms = millis();

  // for (auto packet_data : m_redundant_packet_deliveries)
  // {
  //   if (ms - packet_data.last_delivery_ms >= packet_data.ms_between_deliveries)
  //   {
  //     packet_data.last_delivery_ms = ms;
  //     packet_data.redundant_deliveries--;

  //     send_packet(packet_data.receipt_address, packet_data.packet, false);
  //   }
  // }

  // // Iterate through std::vector in reverse order for safer* deletions, probably.
  // for (size_t i = m_redundant_packet_deliveries.size() - 1; i > 0; i--)
  // {
  //   auto packet_data = m_redundant_packet_deliveries[i];
  //   if (packet_data.redundant_deliveries == 0 || packet_data.redundant_deliveries == 255)
  //     m_redundant_packet_deliveries.erase(m_redundant_packet_deliveries.begin() + i);
  // }
}

void ConeLightNetworking::send_packet(const uint8_t *mac_addr, cone_light_network_packet_t packet, bool redundant_delivery)
{
  strncpy(packet.protocol_id, CONE_LIGHT_NETWORKING_PROTOCOL_ID, 5); // CONE_LIGHT_NETWORKING_PROTOCOL_ID is 4 characters + NULL
  packet.firmware_version = CONE_LIGHT_FIRMWARE_VERSION;
  packet.packet_id = m_packet_id++;
  packet.timestamp = m_cone_light->network_time()->timestamp();
  packet.node_id = m_cone_light->node_id();
  strncpy(packet.node_name, m_cone_light->node_name().c_str(), 7); // names are 6 characters + NULL
  packet.node_group_id = m_cone_light->node_group_id();

  // cone_light_redundant_packet_delivery_t packet_delivery = {};
  // if (redundant_delivery)
  // {
  //   memcpy(packet_delivery.receipt_address, mac_addr, 6);
  //   memcpy(&packet_delivery.packet, &packet, sizeof(cone_light_network_packet_t)); // CHECK: This might segfault
  //   packet_delivery.last_delivery_ms = millis();

  //   m_redundant_packet_deliveries.emplace_back(redundant_delivery);
  // }

  esp_err_t result = esp_now_send(mac_addr, (uint8_t *)&packet, sizeof(packet));

  if (result != ESP_OK)
    Serial.printf("ESPNow failed to send packet: [%d] %s\n", result, esp_err_to_name(result));
}

void ConeLightNetworking::broadcast_packet(cone_light_network_packet_t packet, bool redundant_delivery)
{
  send_packet(m_broadcast_address, packet, redundant_delivery);
}

void ConeLightNetworking::on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
}

void ConeLightNetworking::on_data_received(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len)
{
  // Received packet cannot be handled, doesn't fit into mono packet size
  if (sizeof(cone_light_network_packet_t) != len)
  {
    Serial.printf("Networking: Rejected packet of incorrect payload size: %d, expected: %d\n", len, sizeof(cone_light_network_packet_t));
    return;
  }

  cone_light_network_packet_t packet = {};
  memcpy(&packet, data, sizeof(cone_light_network_packet_t));

  // Received packet cannot be handled, unknown protocol id
  if (strncmp(packet.protocol_id, CONE_LIGHT_NETWORKING_PROTOCOL_ID, 5) != 0)
  {
    Serial.printf("Networking: Rejected packet due to protocol id mismatch. Got: %s, expected: %s\n", packet.protocol_id, CONE_LIGHT_NETWORKING_PROTOCOL_ID);
    return;
  }

  // Received packet cannot be handled, incompatible firmware (protocol) version
  if (packet.firmware_version != CONE_LIGHT_FIRMWARE_VERSION)
  {
    Serial.printf("Networking: Rejected packet due to firmware version mismatch. Got: %u, expected: %u\n", packet.firmware_version, CONE_LIGHT_FIRMWARE_VERSION);
    return;
  }

  // Received packet cannot be handled, node id exceeds max number of allowed nodes
  if (packet.node_id >= CONE_LIGHT_NETWORKING_MAX_NODES)
  {
    Serial.printf("Networking: Rejected packet due to node id exceeding max number of allowed nodes. Got: %u, expected: 0..%u\n", packet.node_id, CONE_LIGHT_NETWORKING_MAX_NODES);
    return;
  }

  // Received packet cannot be handled, node identifier mismatch or repeated packet/command id
  if (!m_known_nodes[packet.node_id].ingest_packet(esp_now_info, packet))
  {
    Serial.printf("Networking: Rejected packet due to node identifier mismatch or repeated packet/command id.\n");
    return;
  }

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

#include "include/cone_light_networking.h"
#include "include/cone_light_web_spa.h"

ConeLightNetworking::ConeLightNetworking(ConeLight *cone_light)
{
  m_cone_light = cone_light;

  // See: https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/
  pinMode(WIFI_ENABLE, OUTPUT);   // pinMode(3, OUTPUT);
  digitalWrite(WIFI_ENABLE, LOW); // digitalWrite(3, LOW); // Activate RF switch control
  delay(100);
  pinMode(WIFI_ANT_CONFIG, OUTPUT);    // pinMode(14, OUTPUT);
  digitalWrite(WIFI_ANT_CONFIG, HIGH); // digitalWrite(14, HIGH); // Use external antenna

  WiFi.mode(cone_light->node_remote() ? WIFI_MODE_APSTA : WIFI_MODE_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set Long Range mode
  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

  esp_now_register_send_cb(cone_light_networking_send_callback);
  esp_now_register_recv_cb(cone_light_networking_recv_callback);

  esp_now_peer_info_t peer_info = {};
  memcpy(peer_info.peer_addr, m_broadcast_address, 6);
  peer_info.channel = 0;
  peer_info.encrypt = false;

  if (esp_now_add_peer(&peer_info) != ESP_OK)
  {
    Serial.println("Failed to add broadcast peer!");

    return;
  }

  if (cone_light->node_remote())
  {
    if (!WiFi.softAP(CONE_LIGHT_NETWORKING_ACCESS_POINT_NAME, cone_light->node_access_point_password()))
    {
      Serial.println("Failed to create AP!");

      return;
    }

    configure_web_server();
  }

  Serial.printf("    Networking initialized successfully. (MAC ADDR: %s)\n", WiFi.macAddress().c_str());
}

void ConeLightNetworking::configure_web_server()
{
  m_web_server = new AsyncWebServer(80);
  m_websocket_handler = new AsyncWebSocketMessageHandler();
  m_websocket = new AsyncWebSocket("/ws", m_websocket_handler->eventHandler());

  m_web_server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    request->send(200, "text/html", data_cone_light_html);
  });

  m_websocket_handler->onConnect([this](AsyncWebSocket *server, AsyncWebSocketClient *client) {
    Serial.printf("WS Client %u connected\n", client->id());
    server->text(client->id(), websocket_metadata_payload());
    server->text(client->id(), websocket_payload());
  });
  m_websocket_handler->onDisconnect([](AsyncWebSocket *server, uint32_t client_id) {
    Serial.printf("WS Client %u disconnected\n", client_id);
  });

  m_websocket_handler->onError([this](AsyncWebSocket *server, AsyncWebSocketClient *client, uint16_t error_code, const char *reason, size_t length) {
    Serial.printf("WS Client %u error: %u: %s\n", client->id(), error_code, reason);
  });

  m_websocket_handler->onMessage([this](AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t length) {
    Serial.printf("WS Client %u data: %s\n", client->id(), (const char *)data);
    handle_websocket(server, client, data, length);
  });

  m_web_server->addHandler(m_websocket);
  m_web_server->begin();
}

void ConeLightNetworking::handle_websocket(AsyncWebSocket *server, AsyncWebSocketClient *client, const uint8_t *data, size_t length)
{
  JsonDocument doc;

  deserializeJson(doc, reinterpret_cast<const char*>(data));

  String command = doc["data"]["command"] | "unknown";
  uint16_t target_group = doc["data"]["target_group"] | 1024;
  uint16_t target_node = doc["data"]["target_node"]   | 1024;
  uint32_t parameter_0 = doc["data"]["parameters"][0] | 0;
  uint32_t parameter_1 = doc["data"]["parameters"][1] | 0;

  cone_light_network_packet_t packet = {};

  if (command == "payload") {
    server->text(client->id(), websocket_payload());

  } else if (command == "color") {
    packet.command_id = m_cone_light->networking()->next_command_id();
    packet.command_type = (target_group == 255) ? ConeLightNetworkCommand::SET_COLOR : ConeLightNetworkCommand::SET_GROUP_COLOR;
    packet.command_parameters = parameter_0;
    packet.command_parameters_extra = (target_group < 255) ? target_group : 255;

    m_cone_light->networking()->broadcast_packet(packet, true);

  } else if (command == "brightness") {

  } else if  (command == "tone") {
    packet.command_id = m_cone_light->networking()->next_command_id();
    packet.command_type = ConeLightNetworkCommand::PLAY_TONE;
    packet.command_parameters = parameter_0;
    packet.command_parameters_extra = (target_group < 255) ? target_group : 255;

    m_cone_light->networking()->broadcast_packet(packet, true);

  } else if  (command == "song") {
    packet.command_id = m_cone_light->networking()->next_command_id();
    packet.command_type = ConeLightNetworkCommand::PLAY_SONG;
    packet.command_parameters = parameter_0;
    packet.command_parameters_extra = (target_group < 255) ? target_group : 255;

    m_cone_light->networking()->broadcast_packet(packet, true);
  }
}

String ConeLightNetworking::websocket_metadata_payload()
{
  JsonDocument doc;
  String json;

  doc["data"]["type"] = "metadata";
  doc["data"]["metadata"]["firmware_version"] = CONE_LIGHT_FIRMWARE_VERSION_NAME;
  doc["data"]["metadata"]["protocol_version"] = CONE_LIGHT_FIRMWARE_VERSION;
  doc["data"]["metadata"]["min_voltage"] = VOLTAGE_MIN;
  doc["data"]["metadata"]["max_voltage"] = VOLTAGE_MAX;
  doc["data"]["metadata"]["groups"][0] = CONE_LIGHT_NODE_GROUP_0_NAME;
  doc["data"]["metadata"]["groups"][1] = CONE_LIGHT_NODE_GROUP_1_NAME;

  JsonArray tones = doc["data"]["metadata"]["tones"].to<JsonArray>();
  for (auto &tone : note_to_freq)
  {
    tones.add(tone);
  }
  JsonArray songs = doc["data"]["metadata"]["songs"].to<JsonArray>();
  for (auto &title : cone_light_song_titles)
  {
    songs.add(title);
  }

  serializeJson(doc, json);

  return json;
}

String ConeLightNetworking::websocket_payload()
{
  JsonDocument doc;
  String json;

  doc["data"]["type"] = "payload";
  doc["data"]["remote"]["id"] = m_cone_light->node_id();
  doc["data"]["remote"]["group_id"] = m_cone_light->node_group_id();
  doc["data"]["remote"]["name"] = m_cone_light->node_name();
  doc["data"]["remote"]["voltage"] = m_cone_light->voltage()->voltage();
  doc["data"]["remote"]["rssi"] = 0.0;

  size_t i = 0;
  for (auto &node : m_known_nodes)
  {
    if (node.node_id == CONE_LIGHT_NODE_ID_UNSET ||
        node.node_group_id == CONE_LIGHT_NODE_GROUP_ID_UNSET)
      continue;

    /* exclude nodes that haven't pong'd or broadcasted in 10 seconds */
    if (m_cone_light->network_time()->timestamp() - node.last_receive_timestamp >= 10'000)
      continue;

    doc["data"]["nodes"][i]["id"] = node.node_id;
    doc["data"]["nodes"][i]["group_id"] = node.node_group_id;
    doc["data"]["nodes"][i]["name"] = node.node_name;
    doc["data"]["nodes"][i]["voltage"] = node.m_voltage;
    doc["data"]["nodes"][i]["rssi"] = node.m_rssi;

    i++;
  }

  serializeJson(doc, json);

  return json;
}

void ConeLightNetworking::update()
{
  uint32_t ms = millis();

  for (auto &package : m_redundant_packet_deliveries)
  {
    if (ms - package->last_delivery_ms() >= package->ms_between_deliveries())
    {
      if (CONE_LIGHT_DEBUG)
        Serial.printf("REDUNDANT PACKET DELIVERY: PENDING ATTEMPTS: %u, NODE ID: %u, PKT ID: %u, CMD ID: %u\n", package->redundant_deliveries(), package->packet().node_id, package->packet().packet_id, package->packet().packet_id);

      send_packet(package->receipt_address(), package->packet(), false);
      package->delivery_attempted();
    }
  }

  m_redundant_packet_deliveries.erase(std::remove_if(m_redundant_packet_deliveries.begin(), m_redundant_packet_deliveries.end(),
                                                     [](auto package)
                                                     { return package->redundant_deliveries() == 0; }),
                                      m_redundant_packet_deliveries.end());
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

  if (redundant_delivery)
  {
    // FIXME: Remove pre-existing queued packets that have the same destination address and command TYPE, to prevent resending no longer needed packets
    m_redundant_packet_deliveries.emplace_back(std::make_shared<ConeLightNetworkingRedundantPacketDelivery>(mac_addr, packet, 2, 5, millis()));
  }

  esp_err_t result = esp_now_send(mac_addr, (uint8_t *)&packet, sizeof(packet));

  if (result != ESP_OK)
    Serial.printf("ESPNow failed to send packet: [%d] %s\n", result, esp_err_to_name(result));
}

void ConeLightNetworking::broadcast_packet(cone_light_network_packet_t packet, bool redundant_delivery)
{
  send_packet(m_broadcast_address, packet, redundant_delivery);
}

bool ConeLightNetworking::add_peer(const esp_now_recv_info_t *esp_now_info)
{
  esp_now_peer_info_t peer_info = {};
  memcpy(peer_info.peer_addr, esp_now_info->src_addr, 6);
  peer_info.channel = 0;
  peer_info.encrypt = false;

  esp_err_t result = esp_now_add_peer(&peer_info);

  return (result == ESP_OK || result == ESP_ERR_ESPNOW_EXIST);
}

void ConeLightNetworking::on_data_sent(const esp_now_send_info_t *esp_now_info, esp_now_send_status_t status)
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

  // packet accepted!
  add_peer(esp_now_info);

  // Handle PING / PONG commands
  switch (packet.command_type)
  {
  case ConeLightNetworkCommand::PING:
    handle_ping(packet);
    break;
  case ConeLightNetworkCommand::PONG:
    handle_pong(packet);
    break;

  default:
    break;
  }

  m_cone_light->espnow_event(packet);
}

// We've received a PING from some node, reply!
void ConeLightNetworking::handle_ping(const cone_light_network_packet_t &packet)
{
  // Ignore our own pings
  if (packet.node_id == m_cone_light->node_id())
    return;

  uint32_t packed_voltage;
  const float voltage = m_cone_light->voltage()->voltage();
  memcpy(&packed_voltage, &voltage, sizeof(float));

  cone_light_network_packet_t pong_pkt{
      .command_id = next_command_id(),
      .command_type = ConeLightNetworkCommand::PONG,
      .command_parameters = packed_voltage,
      .command_parameters_extra = m_cone_light->speaker()->playing()};

  // reply to sending node, unreliably.
  send_packet(node_address(packet.node_id), pong_pkt, false);
}

// We've received a reply to OUR PING, store data!
void ConeLightNetworking::handle_pong(const cone_light_network_packet_t &packet)
{
  auto &node = m_known_nodes[packet.node_id];
  float voltage;
  memcpy(&voltage, &packet.command_parameters, sizeof(float));

  node.m_voltage = voltage;
  node.m_playing_song = packet.command_parameters_extra;
}

//--- non-member functions ---//
extern ConeLight *g_cone_light;
void cone_light_networking_send_callback(const esp_now_send_info_t *esp_now_info, esp_now_send_status_t status)
{
  g_cone_light->networking()->on_data_sent(esp_now_info, status);
}

void cone_light_networking_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int len)
{
  g_cone_light->networking()->on_data_received(esp_now_info, data, len);
}

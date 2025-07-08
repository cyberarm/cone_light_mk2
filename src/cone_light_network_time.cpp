#include "cone_light_network_time.h"

ConeLightNetworkTime::ConeLightNetworkTime(ConeLight *cone_light)
{
  m_cone_light = cone_light;
}

ConeLightNetworkTime::~ConeLightNetworkTime()
{
}

void ConeLightNetworkTime::broadcast_grandmaster_sync()
{
  m_last_time_grandmaster_sync_ms = millis();

  cone_light_network_packet_t packet = {};
  packet.command_id = m_cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::CLOCK_GRANDMASTER_SYNC;

  // ConeLightNetworking will set packet timestamp when sending
  m_cone_light->networking()->broadcast_packet(packet);
}

void ConeLightNetworkTime::node_delay_request()
{
  m_last_time_node_delay_request_ms = millis();

  cone_light_network_packet_t packet = {};
  packet.command_id = m_cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::CLOCK_NODE_DELAY_REQUEST;
  packet.command_parameters = millis();

  // ConeLightNetworking will set packet timestamp when sending
  m_cone_light->networking()->broadcast_packet(packet);
}

void ConeLightNetworkTime::node_delay_response(const uint32_t timestamp)
{
  cone_light_network_packet_t packet = {};
  packet.command_id = m_cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::CLOCK_NODE_DELAY_RESPONSE;
  packet.command_parameters = timestamp;

  // ConeLightNetworking will set packet timestamp when sending
  m_cone_light->networking()->broadcast_packet(packet);
}

void ConeLightNetworkTime::update()
{
  if (is_grandmaster_clock())
  {
    if (millis() - m_last_time_grandmaster_sync_ms < m_node_delay_request_interval_ms)
      return;

    broadcast_grandmaster_sync();
  }
  else
  {
    if (millis() - m_last_time_grandmaster_sync_ms < m_node_delay_request_interval_ms)
      return;

    node_delay_request();
  }
}

bool ConeLightNetworkTime::packet_handler(cone_light_network_packet_t packet)
{
  switch (packet.command_type)
  {
  case CLOCK_GRANDMASTER_SYNC:
    // Ignore echos of the past (or competing grandmasters that shouldn't exist)
    if (is_grandmaster_clock())
      break;

    m_grandmaster_offset_ms = millis() - packet.timestamp;
    return true;
    break;

  case CLOCK_NODE_DELAY_REQUEST:
    // Ignore other nodes cry for time unless we're the Grand Master Clock
    if (!is_grandmaster_clock())
      break;
      node_delay_response(packet.timestamp);
    break;

  case CLOCK_NODE_DELAY_RESPONSE:
    // Grandmaster does not need this, probably.
    if (is_grandmaster_clock())
      break;

    m_node_delay_offset_ms = packet.command_parameters;
    m_offset_ms = (m_grandmaster_offset_ms - m_node_delay_offset_ms) / 2;
    m_clock_synced = true;
    return true;
    break;

  default:
    break;
  }

  return false;
}

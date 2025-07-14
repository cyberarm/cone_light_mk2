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
  m_node_delay_request_time_ms = millis();

  cone_light_network_packet_t packet = {};
  packet.command_id = m_cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::CLOCK_NODE_DELAY_REQUEST;
  packet.command_parameters = m_node_delay_request_time_ms;

  // ConeLightNetworking will set packet timestamp when sending
  m_cone_light->networking()->broadcast_packet(packet);
}

void ConeLightNetworkTime::node_delay_response(const cone_light_network_packet_t request)
{
  cone_light_network_packet_t packet = {};
  packet.command_id = m_cone_light->networking()->next_command_id();
  packet.command_type = ConeLightNetworkCommand::CLOCK_NODE_DELAY_RESPONSE;
  packet.command_parameters = request.timestamp;
  packet.command_parameters_extra = request.node_id;

  // ConeLightNetworking will set packet timestamp when sending
  // TODO: Send packet directly to node instead of broadcasting to all nodes.
  m_cone_light->networking()->broadcast_packet(packet);
}

void ConeLightNetworkTime::update()
{
  // Sync state in case the node has been reconfigured
  m_node_grandmaster_clock = m_cone_light->node_grandmaster_clock();

  if (is_grandmaster_clock())
  {
    // Keep our grandmaster clock node's idea of time up to date
    m_grandmaster_time = millis();
    m_grandmaster_time_accumulator_ms = 0;
    m_last_grandmaster_time_received_ms = 0;

    // Check whether its time to announce the time
    if (millis() - m_last_time_grandmaster_sync_ms < m_grandmaster_sync_interval_ms)
      return;

    broadcast_grandmaster_sync();
  }
  else
  {
    // Track duration between last received grand master time
    m_grandmaster_time_accumulator_ms = millis() - m_last_grandmaster_time_received_ms;

    // Check whether its time to refresh the node's ping offset to the grandmaster
    if (millis() - m_last_time_node_delay_request_ms < m_node_delay_request_interval_ms)
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

    // grand master clock has been restarted, force re-sync
    if (m_clock_synced && packet.timestamp < m_grandmaster_time)
      m_clock_synced = false;

    // If clock hasn't synced yet, then simply overwrite synced time with grand master's time to start things off
    if (!m_clock_synced)
      m_grandmaster_time = packet.timestamp;

    // IDEA: It may be be better to use time() sans `m_offset_ms` for a possibly more accurate grand master offset
    //       (double integration and all that)
    m_grandmaster_offset_ms = time() - packet.timestamp;
#if CONE_LIGHT_DEBUG
    Serial.printf("Node clock error: %d ms\n", m_grandmaster_offset_ms);
#endif

    m_grandmaster_time = packet.timestamp;
    m_last_grandmaster_time_received_ms = millis();

    // Eagerly request time to ping grandmaster when clock is not yet synced
    if (!m_clock_synced)
      node_delay_request();

    // Recalculate grand master clock offset
    if (m_clock_synced)
      calculate_clock_offset();

    return true;
    break;

  case CLOCK_NODE_DELAY_REQUEST:
    // Ignore other nodes cry for time unless we're the Grand Master Clock
    if (!is_grandmaster_clock())
      break;

    node_delay_response(packet);
    break;

  case CLOCK_NODE_DELAY_RESPONSE:
    // Grandmaster does not need this, probably.
    if (is_grandmaster_clock())
      break;
    // This message is not meant for this node
    if (packet.command_parameters_extra != m_cone_light->node_id())
      break;

    // flip sign since this is inverted from reference. We're using the local node's time to 'ping'
    // the grand master instead of using the grand master's clock as time reference.
    m_node_delay_offset_ms = (millis() - m_node_delay_request_time_ms) * -1;
#if CONE_LIGHT_DEBUG
    Serial.printf("Node clock delay: %d ms\n", m_node_delay_offset_ms);
#endif

    // get half of 'ping' time to grand master clock
    calculate_clock_offset();
    m_clock_synced = true;

#if CONE_LIGHT_DEBUG
    Serial.printf("Node clock calculated offset: %d ms (grand offset: %d ms, ping offset: %d ms)\n", m_offset_ms, m_grandmaster_offset_ms, m_node_delay_offset_ms);
#endif
    return true;
    break;

  default:
    break;
  }

  return false;
}

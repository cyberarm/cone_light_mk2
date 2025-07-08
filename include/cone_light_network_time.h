#pragma once

#include <Arduino.h>
#include "cone_light.h"

// Implements PTP (or an inferior version of it :) )
// REF: https://www.bodet-time.com/resources/blog/1774-how-works-the-precision-time-protocol-ptp.html
class ConeLightNetworkTime
{
private:
  const uint32_t m_grandmaster_sync_interval_ms = 2'000;
  const uint32_t m_node_delay_request_interval_ms = 60'000;

  ConeLight *m_cone_light = nullptr;

  uint32_t m_grandmaster_offset_ms = 0; // Offset of local clock and grandmaster
  uint32_t m_node_delay_offset_ms = 0;  // Offset of time to 'ping' grandmaster
  uint32_t m_offset_ms = 0;             // Unified offset
  uint32_t m_last_time_grandmaster_sync_ms = 0;
  uint32_t m_last_time_node_delay_request_ms = 0;
  bool m_node_grandmaster_clock = false;
  bool m_clock_synced = false;

  void broadcast_grandmaster_sync();
  void node_delay_request();
  void node_delay_response(const uint32_t timestamp);

public:
  ConeLightNetworkTime(ConeLight *cone_light);
  ~ConeLightNetworkTime();

  void update();
  bool packet_handler(cone_light_network_packet_t packet);
  bool is_grandmaster_clock() { return m_node_grandmaster_clock; };
  // Grandmaster clock is always in sync because it is the Grand Master of Time :)
  bool is_clock_synced() { return is_grandmaster_clock() || m_clock_synced; };
  uint32_t timestamp() { return millis() + m_offset_ms; };
};

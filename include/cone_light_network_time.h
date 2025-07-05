#pragma once

#include <cinttypes>
#include "cone_light_enums.h"
#include "cone_light_network_packet.h"

// Implements PTP (or an inferior version of it :) )
// REF: https://www.bodet-time.com/resources/blog/1774-how-works-the-precision-time-protocol-ptp.html
class ConeLightNetworkTime
{
private:
  uint32_t m_offset_ms = 0;
  bool m_node_master_clock = false;
  bool m_clock_synced = false;

public:
  ConeLightNetworkTime();
  ~ConeLightNetworkTime();

  void update();
  bool packet_handler(cone_light_network_packet_t packet);
  bool is_master_clock() { return m_node_master_clock; };
  // Master clock is always in sync because it is the Master of Time :)
  bool is_clock_synced() { return is_master_clock() || m_clock_synced; };
};

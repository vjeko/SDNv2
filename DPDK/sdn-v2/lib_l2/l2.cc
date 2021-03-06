#include <iostream>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_log.h>

#include <map>
#include <defs.h>
#include "l2.hh"

namespace dpdk {

std::string L2Switch::print_ether_addr(ether_addr addr) {

  char buffer[32];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
      addr.addr_bytes[0],
      addr.addr_bytes[1],
      addr.addr_bytes[2],
      addr.addr_bytes[3],
      addr.addr_bytes[4],
      addr.addr_bytes[5]);

  return buffer;
}



int L2Switch::component_learn(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  const uint32_t portid = m->pkt.in_port;
  const ether_hdr* ether = (ether_hdr*) m->pkt.data;

  ether_addr_union src_addr_union;
  src_addr_union.as_addr = ether->s_addr;

  const uint64_t src_addr_int = src_addr_union.as_int;
  if (src_addr_int == L2_BROADCAST) return 0;

  auto query = l2_addr_map_.find(src_addr_int);
  if (query == l2_addr_map_.end()) {
    query = l2_addr_map_.insert(std::make_pair(src_addr_int, portid)).first;
    const string host = print_ether_addr(ether->s_addr);
    RTE_LOG(INFO, LIB_SWITCH, "Discovered %s at port %d\n", host.c_str(), portid);
  }

  return 0;
}



uint32_t L2Switch::find(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  const ether_hdr* ether = (ether_hdr*) m->pkt.data;

  ether_addr_union dst_addr_union;
  dst_addr_union.as_addr = ether->d_addr;

  const uint64_t dst_addr_int = dst_addr_union.as_int;

  if (dst_addr_int == L2_BROADCAST)
    return FLOOD_PORT;

  auto query = l2_addr_map_.find(dst_addr_int);
  if (query == l2_addr_map_.end()) {
    const string host = print_ether_addr(ether->d_addr);
    RTE_LOG(INFO, LIB_SWITCH, "Unable to locate %s\n", host.c_str());
  }

  return query->second;
}



int L2Switch::component_route(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  uint32_t output_port = find(m, qconf);

  rte_pktmbuf_prepend(m, sizeof(port_t));
  port_t* port = (port_t*) m->pkt.data;
  *port = output_port;

  return 0;
}


}

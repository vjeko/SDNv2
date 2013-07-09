/*
 * module.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: vjeko
 */

#include <iostream>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_log.h>

#include <map>
#include <defs.h>

using std::string;

static std::map<uint64_t, size_t> l2_addr_map_;


std::string print_ether_addr(ether_addr addr) {

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


extern "C" void learn(struct rte_mbuf *m, unsigned portid) {

  const ether_hdr* ether = (ether_hdr*) m->pkt.data;

  ether_addr_union src_addr_union;
  src_addr_union.as_addr = ether->s_addr;

  const uint64_t src_addr_int = src_addr_union.as_int;
  if (src_addr_int == L2_BROADCAST) return;

  auto query = l2_addr_map_.find(src_addr_int);
  if (query == l2_addr_map_.end()) {
    query = l2_addr_map_.insert(std::make_pair(src_addr_int, portid)).first;
    const string host = print_ether_addr(ether->s_addr);
    RTE_LOG(INFO, LIB_SWITCH, "Discovered %s at port %d\n", host.c_str(), portid);
  }

}



extern "C" unsigned decide(struct rte_mbuf *m) {

  const ether_hdr* ether = (ether_hdr*) m->pkt.data;

  ether_addr_union dst_addr_union;
  dst_addr_union.as_addr = ether->d_addr;

  const uint64_t dst_addr_int = dst_addr_union.as_int;

  if (dst_addr_int == L2_BROADCAST)
    return FLOOD_PORT;

  auto query = l2_addr_map_.find(dst_addr_int);
  if (query == l2_addr_map_.end()) {
    const string host = print_ether_addr(ether->d_addr);
    RTE_LOG(INFO, LIB_SWITCH, "Unable to locate %sn", host.c_str());
  }

  return query->second;
}



extern "C" uint32_t l2_switch(struct rte_mbuf *m, uint32_t portid) {
  learn(m, portid);
  return decide(m);
}

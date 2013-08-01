/*
 * module.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: vjeko
 */

#include <iostream>
#include <array>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_ether.h>
#include <rte_log.h>

#include <defs.h>
#include <endian.h>

#include "ethane.h"


using std::string;
using std::array;

namespace dpdk {



int Ethane::action_classify_forbidden(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  rte_pktmbuf_prepend(m, sizeof(EthaneMetaHeader));
  EthaneMetaHeader* header = (EthaneMetaHeader*) m->pkt.data;
  header->class_ = CLASS::FORBID;

  return PIPELINE::CONTINUE;
}



void Ethane::component_ethane_init() {

  union AddressUnion {
    array<uint8_t, sizeof(uint32_t)> array_;
    uint32_t int_;
  };

  AddressUnion src_addr, dst_addr;
  src_addr.int_ = be32toh( IPv4(10,10,10,11) );
  dst_addr.int_ = be32toh( IPv4(10,10,10,12) );

  Action* action = classifier.add_rule(src_addr.array_, dst_addr.array_);
  action->type_ = ACTION::CLASSIFY;
  action->callback_ = &Ethane::action_classify_forbidden;
}



int Ethane::component_ethane_class(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  const ether_hdr* ether = (ether_hdr*) &(m->pkt.data);
  const ipv4_hdr* ip = (ipv4_hdr*)
      (rte_pktmbuf_mtod(m, unsigned char*) + sizeof(ether_hdr));

  //printf("%x -> %x\n", htobe32(ip->src_addr), htobe32(ip->dst_addr));

  dpdk::Action* action = classifier.find(m->pkt.data);
  if (action) return action->callback_(m, qconf);

  rte_pktmbuf_prepend(m, sizeof(EthaneMetaHeader));
  EthaneMetaHeader* header = (EthaneMetaHeader*) m->pkt.data;
  header->class_ = CLASS::ALLOW;

  return PIPELINE::CONTINUE;
}



int Ethane::component_ethane_action(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  const EthaneMetaHeader* header = ( (EthaneMetaHeader*) m->pkt.data );
  if (header->class_ == CLASS::ALLOW) {

    rte_pktmbuf_adj(m, sizeof(EthaneMetaHeader));
    return PIPELINE::CONTINUE;

  } else if (header->class_ == CLASS::FORBID) {

    rte_pktmbuf_free(m);
    return PIPELINE::STOP;

  } else {

    printf("Unknown classification.\n");
    rte_pktmbuf_free(m);
    return PIPELINE::STOP;

  }

}


}

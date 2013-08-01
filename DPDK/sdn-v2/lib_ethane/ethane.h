/*
 * module.h
 *
 *  Created on: Jul 3, 2013
 *      Author: vjeko
 */

#ifndef ETHANE_H_
#define ETHANE_H_

#include <rte_mbuf.h>
#include <rte_ip.h>
#include <rte_ether.h>

#include <defs.h>
#include <lib_classifier/classifier.hh>

namespace dpdk {



enum ACTION {
  CLASSIFY = 1
};

enum PIPELINE {
  CONTINUE = 0,
  STOP = 1
};

enum CLASS {
  ALLOW,
  FORBID
};



struct EthaneMetaHeader {
  uint32_t class_;
} __attribute__((__packed__));



class Ethane {
public:

  static int action_classify_forbidden(
      struct rte_mbuf *m,
      struct lcore_queue_conf *qconf);

  void component_ethane_init(void);

  int component_ethane_class(
      struct rte_mbuf *m,
      struct lcore_queue_conf *qconf);
  int component_ethane_action(
      struct rte_mbuf *m,
      struct lcore_queue_conf *qconf);

private:

  static const size_t src_offset = sizeof(ether_hdr) + offsetof(ipv4_hdr, src_addr);
  static const size_t dst_offset = sizeof(ether_hdr) + offsetof(ipv4_hdr, dst_addr);

  static const size_t src_size = sizeof ipv4_hdr().src_addr;
  static const size_t dst_size = sizeof ipv4_hdr().dst_addr;

  typedef dpdk::Match<src_size, src_offset> SrcMatch;
  typedef dpdk::Match<dst_size, dst_offset> DstMatch;

  dpdk::Classifier<
      dpdk::MatchExact<SrcMatch>,
      dpdk::MatchExact<DstMatch>
    > classifier;
};

}

#endif /* MODULE_H_ */

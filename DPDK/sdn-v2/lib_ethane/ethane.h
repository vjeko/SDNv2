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


  Ethane() : classify_(*this), enforce_(*this) {}

  static int action_classify_forbidden(
      struct rte_mbuf *m,
      struct lcore_queue_conf *qconf);

  void component_ethane_init(void);




  class Classify {
  public:

    Classify(Ethane& ethane) : parent_(ethane) {};

    int component(
        struct rte_mbuf *m,
        struct lcore_queue_conf *qconf);

    Ethane& parent_;
  };




  class Enforce {
  public:

    Enforce(Ethane& ethane) : parent_(ethane) {};

    int component(
        struct rte_mbuf *m,
        struct lcore_queue_conf *qconf);

    Ethane& parent_;
  };



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

  Classify classify_;
  Enforce enforce_;
};

}

#endif /* MODULE_H_ */

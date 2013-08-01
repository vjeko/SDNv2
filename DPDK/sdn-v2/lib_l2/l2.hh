/*
 * module.h
 *
 *  Created on: Jul 3, 2013
 *      Author: vjeko
 */

#ifndef MODULE_H_
#define MODULE_H_

#include "defs.h"
#include <rte_config.h>
#include <rte_mbuf.h>

#include <string>
#include <unordered_map>

namespace dpdk {

using std::string;
using std::unordered_map;

typedef uint32_t port_t;


class L2Switch {
public:

  string print_ether_addr(ether_addr addr);

  int component_learn(struct rte_mbuf *m, struct lcore_queue_conf *qconf);
  int component_route(struct rte_mbuf *m, struct lcore_queue_conf *qconf);
  uint32_t find(struct rte_mbuf *m, struct lcore_queue_conf *qconf);


  unordered_map<uint64_t, size_t> l2_addr_map_;
};

}

#endif /* MODULE_H_ */

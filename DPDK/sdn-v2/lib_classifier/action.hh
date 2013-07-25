/*
 * action.hh
 *
 *  Created on: Jul 23, 2013
 *      Author: vjeko
 */

#ifndef ACTION_HH_
#define ACTION_HH_

#include <rte_config.h>
#include <rte_mbuf.h>

namespace dpdk {

struct Action {

  size_t type_;
  int (*callback_)(struct rte_mbuf *m, struct lcore_queue_conf *qconf);

};

}

#endif /* ACTION_HH_ */

/*
 * module.h
 *
 *  Created on: Jul 3, 2013
 *      Author: vjeko
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <rte_config.h>
#include <rte_mbuf.h>

extern void component_learn(struct rte_mbuf *m, struct lcore_queue_conf *qconf);
extern void component_route(struct rte_mbuf *m, struct lcore_queue_conf *qconf);
extern uint32_t find(struct rte_mbuf *m, struct lcore_queue_conf *qconf);


#endif /* MODULE_H_ */

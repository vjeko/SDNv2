/*
 * module.h
 *
 *  Created on: Jul 3, 2013
 *      Author: vjeko
 */

#ifndef ETHANE_H_
#define ETHANE_H_

#include "defs.h"
#include <rte_config.h>
#include <rte_mbuf.h>

extern int action_classify_forbidden(struct rte_mbuf *m, struct lcore_queue_conf *qconf);

extern void component_ethane_init(void);
extern int component_ethane_class(struct rte_mbuf *m, struct lcore_queue_conf *qconf);
extern int component_ethane_action(struct rte_mbuf *m, struct lcore_queue_conf *qconf);


#endif /* MODULE_H_ */

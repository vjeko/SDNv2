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

extern uint32_t l2_switch(struct rte_mbuf *m, unsigned portid);

#endif /* MODULE_H_ */

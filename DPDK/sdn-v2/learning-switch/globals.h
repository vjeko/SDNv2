/*
 * globals.h
 *
 *  Created on: Jul 6, 2013
 *      Author: vjeko
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include "defs.h"

extern uint16_t nb_rxd;
extern uint16_t nb_txd;

struct ether_addr ports_eth_addr[MAX_PORTS]; /* ethernet addresses of ports */
extern uint32_t enabled_port_mask; /* mask of enabled ports */

uint32_t dst_ports[MAX_PORTS]; /* list of enabled ports */

extern unsigned int rx_queue_per_lcore;

struct lcore_queue_conf lcore_queue_conf[RTE_MAX_LCORE];

extern const struct rte_eth_conf port_conf;
extern const struct rte_eth_rxconf rx_conf;
extern const struct rte_eth_txconf tx_conf;

struct rte_mempool *packet_pool, *header_pool, *clone_pool;
struct port_statistics port_statistics[MAX_PORTS];

extern int64_t timer_period;


#endif /* GLOBALS_H_ */

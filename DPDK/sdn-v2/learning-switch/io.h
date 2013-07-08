/*
 * io.h
 *
 *  Created on: Jul 7, 2013
 *      Author: vjeko
 */

#ifndef IO_H_
#define IO_H_

#include <stdint.h>

void send_burst(
    struct lcore_queue_conf *qconf,
    uint8_t port);

inline struct rte_mbuf * pkt_duplicate(
    struct rte_mbuf *pkt,
    int use_clone);

inline void pkt_send_single(
    struct rte_mbuf *pkt,
    struct lcore_queue_conf *qconf,
    uint8_t port);

inline void pkt_flood(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf);

#endif /* IO_H_ */
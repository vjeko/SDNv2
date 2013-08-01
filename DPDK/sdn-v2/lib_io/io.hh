/*
 * io.h
 *
 *  Created on: Jul 7, 2013
 *      Author: vjeko
 */

#ifndef IO_H_
#define IO_H_

#include <rte_config.h>
#include <rte_mbuf.h>

#include <stdio.h>
#include <stdint.h>

#include <defs.h>
#include <globals.h>


class IO {
public:

  static void send_burst(
    struct lcore_queue_conf *qconf,
    uint8_t port);

  static struct rte_mbuf * pkt_duplicate(
    struct rte_mbuf *pkt,
    int use_clone);

  static void pkt_send_single(
    struct rte_mbuf *pkt,
    struct lcore_queue_conf *qconf,
    uint8_t port);

  static void pkt_flood(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf);

  static int component_output(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf);
  };


#endif /* IO_H_ */

/*
 * defs.h
 *
 *  Created on: Jul 6, 2013
 *      Author: vjeko
 */

#ifndef DEFS_H_
#define DEFS_H_

#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

#define RTE_LOGTYPE_SWITCH RTE_LOGTYPE_USER1
#define RTE_LOGTYPE_LIB_SWITCH RTE_LOGTYPE_USER1


#define MAX_PORTS 16
#define L2FWD_MAX_PORTS 32

#define MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define NB_MBUF   8192

#define MCAST_CLONE_PORTS 2
#define MCAST_CLONE_SEGS  2

#define PKT_MBUF_SIZE (2048 + sizeof(struct rte_mbuf) + RTE_PKTMBUF_HEADROOM)
#define NB_PKT_MBUF 8192

#define HDR_MBUF_SIZE (sizeof(struct rte_mbuf) + 2 * RTE_PKTMBUF_HEADROOM)
#define NB_HDR_MBUF (NB_PKT_MBUF * MAX_PORTS)

#define CLONE_MBUF_SIZE (sizeof(struct rte_mbuf))
#define NB_CLONE_MBUF (NB_PKT_MBUF * MCAST_CLONE_PORTS * MCAST_CLONE_SEGS * 2)

/*
 * RX and TX Prefetch, Host, and Write-back threshold values should be
 * carefully set for optimal performance. Consult the network
 * controller's datasheet and supporting DPDK documentation for guidance
 * on how these parameters should be set.
 */
#define RX_PTHRESH 8 /**< Default values of RX prefetch threshold reg. */
#define RX_HTHRESH 8 /**< Default values of RX host threshold reg. */
#define RX_WTHRESH 4 /**< Default values of RX write-back threshold reg. */

/*
 * These default values are optimized for use with the Intel(R) 82599 10 GbE
 * Controller and the DPDK ixgbe PMD. Consider using other values for other
 * network controllers and/or network drivers.
 */
#define TX_PTHRESH 36 /**< Default values of TX prefetch threshold reg. */
#define TX_HTHRESH 0  /**< Default values of TX host threshold reg. */
#define TX_WTHRESH 0  /**< Default values of TX write-back threshold reg. */

#define MAX_PKT_BURST 32
#define BURST_TX_DRAIN 20000ULL /* around 100us at 2 Ghz */

#define SOCKET0 0
#define PRINT_STATS 0
/*
 * Configurable number of RX/TX ring descriptors
 */
#define RTE_TEST_RX_DESC_DEFAULT 128
#define RTE_TEST_TX_DESC_DEFAULT 512

#define MAX_PKT_BURST 32

#define MAX_RX_QUEUE_PER_LCORE 16
#define MAX_TX_QUEUE_PER_PORT 16

/* A tsc-based timer responsible for triggering statistics printout */
#define TIMER_MILLISECOND 20000000ULL /* around 1ms at 2 Ghz */
#define MAX_TIMER_PERIOD 86400 /* 1 day max */

#define L2_BROADCAST 0xFFFFFFFFFFFF
#define FLOOD_PORT   UINT32_MAX
#define DROP_PACKET  UINT32_MAX - 1

struct mbuf_table {
  unsigned len;
  struct rte_mbuf *m_table[MAX_PKT_BURST];
};

struct lcore_queue_conf {
  uint64_t tx_tsc;
  uint16_t n_rx_queue;
  uint8_t rx_queue_list[MAX_RX_QUEUE_PER_LCORE];
  uint16_t tx_queue_id[MAX_PORTS];
  struct mbuf_table tx_mbufs[MAX_PORTS];
} __rte_cache_aligned;

/* Per-port statistics struct */
struct port_statistics {
  uint64_t tx;
  uint64_t rx;
  uint64_t dropped;
} __rte_cache_aligned;

union ether_addr_union {
  uint64_t as_int;
  struct ether_addr as_addr;
};

#endif /* DEFS_H_ */

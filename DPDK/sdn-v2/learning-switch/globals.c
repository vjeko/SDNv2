#include <stdint.h>

#include <rte_ethdev.h>

#include "defs.h"

uint16_t nb_rxd = RTE_TEST_RX_DESC_DEFAULT;
uint16_t nb_txd = RTE_TEST_TX_DESC_DEFAULT;

unsigned int rx_queue_per_lcore = 1;
int64_t timer_period = 10 * TIMER_MILLISECOND * 1000; /* default period is 10 seconds */
uint32_t enabled_port_mask = 0;


const struct rte_eth_conf port_conf = {
  .rxmode = {
    .split_hdr_size = 0,
    .header_split   = 0, /**< Header Split disabled */
    .hw_ip_checksum = 0, /**< IP checksum offload disabled */
    .hw_vlan_filter = 0, /**< VLAN filtering disabled */
    .jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
    .hw_strip_crc   = 0, /**< CRC stripped by hardware */
  },
  .txmode = {
    .mq_mode = ETH_DCB_NONE,
  },
};

const struct rte_eth_txconf tx_conf = {
  .tx_thresh = {
    .pthresh = TX_PTHRESH,
    .hthresh = TX_HTHRESH,
    .wthresh = TX_WTHRESH,
  },
  .tx_free_thresh = 0, /* Use PMD default values */
  .tx_rs_thresh = 0, /* Use PMD default values */
};

const struct rte_eth_rxconf rx_conf = {
  .rx_thresh = {
    .pthresh = RX_PTHRESH,
    .hthresh = RX_HTHRESH,
    .wthresh = RX_WTHRESH,
  },
};


#include <rte_config.h>
#include <rte_cycles.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

#include "config.h"
#include "globals.h"
#include "defs.h"

#include <getopt.h>
#include <stdio.h>
#include <string.h>

void init_eal(void) {

  /* create the mbuf pools */
  packet_pool = rte_mempool_create("packet_pool", NB_PKT_MBUF,
      PKT_MBUF_SIZE, 32, sizeof(struct rte_pktmbuf_pool_private),
      rte_pktmbuf_pool_init, NULL, rte_pktmbuf_init, NULL,
      SOCKET0, 0);

  if (packet_pool == NULL)
    rte_exit(EXIT_FAILURE, "Cannot init packet mbuf pool\n");

  header_pool = rte_mempool_create("header_pool", NB_HDR_MBUF,
      HDR_MBUF_SIZE, 32, 0, NULL, NULL, rte_pktmbuf_init, NULL,
      SOCKET0, 0);

  if (header_pool == NULL)
    rte_exit(EXIT_FAILURE, "Cannot init header mbuf pool\n");

  clone_pool = rte_mempool_create("clone_pool", NB_CLONE_MBUF,
      CLONE_MBUF_SIZE, 32, 0, NULL, NULL, rte_pktmbuf_init, NULL,
      SOCKET0, 0);

  if (clone_pool == NULL)
    rte_exit(EXIT_FAILURE, "Cannot init clone mbuf pool\n");

  /* init driver(s) */
  if (rte_pmd_init_all() < 0)
    rte_exit(EXIT_FAILURE, "Cannot init pmd\n");

  if (rte_eal_pci_probe() < 0)
    rte_exit(EXIT_FAILURE, "Cannot probe PCI\n");
}



void init_ports(void) {

  unsigned int nb_ports;

  nb_ports = rte_eth_dev_count();
  if (nb_ports == 0)
    rte_exit(EXIT_FAILURE, "No Ethernet ports - bye\n");

  if (nb_ports > MAX_PORTS)
    nb_ports = MAX_PORTS;


  uint8_t portid;
  struct rte_eth_dev_info dev_info;
  unsigned nb_ports_in_mask = 0;

  /* Each logical core is assigned a dedicated TX queue on each port. */
  for (portid = 0; portid < nb_ports; portid++) {
    /* skip ports that are not enabled */
    if ((enabled_port_mask & (1 << portid)) == 0)
      continue;

    nb_ports_in_mask++;
    rte_eth_dev_info_get(portid, &dev_info);
  }


  struct lcore_queue_conf *qconf = NULL;
  unsigned rx_lcore_id = 0;

  /* Initialize the port/queue configuration of each logical core */
  for (portid = 0; portid < nb_ports; portid++) {
    /* skip ports that are not enabled */
    if ((enabled_port_mask & (1 << portid)) == 0)
      continue;

    /* get the lcore_id for this port */
    while (rte_lcore_is_enabled(rx_lcore_id) == 0 ||
           lcore_queue_conf[rx_lcore_id].n_rx_queue == rx_queue_per_lcore) {

      rx_lcore_id++;
      if (rx_lcore_id >= RTE_MAX_LCORE)
        rte_exit(EXIT_FAILURE, "Not enough cores\n");
    }

    if (qconf != &lcore_queue_conf[rx_lcore_id])
      /* Assigned a new logical core in the loop above. */
      qconf = &lcore_queue_conf[rx_lcore_id];

    qconf->rx_queue_list[qconf->n_rx_queue] = portid;
    qconf->n_rx_queue++;
  }

  /* Initialize each port */
  for (portid = 0; portid < nb_ports; portid++) {
    /* skip ports that are not enabled */
    if ((enabled_port_mask & (1 << portid)) == 0)
      continue;

    /* init port */
    printf("Initializing port %u... ", portid);
    fflush(stdout);
    int ret;

    ret = rte_eth_dev_configure(portid, 1, 1, &port_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "Cannot configure device: err=%d, port=%u\n", ret, portid);

    rte_eth_macaddr_get(portid, &ports_eth_addr[portid]);

    /* init one RX queue */
    ret = rte_eth_rx_queue_setup(portid, 0, nb_rxd, SOCKET0, &rx_conf, packet_pool);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n", ret, portid);

    /* init one TX queue logical core on each port */
    ret = rte_eth_tx_queue_setup(portid, 0, nb_txd, SOCKET0, &tx_conf);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "rte_eth_tx_queue_setup:err=%d, port=%u\n", ret, portid);

    /* Start device */
    ret = rte_eth_dev_start(portid);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "rte_eth_dev_start:err=%d, port=%u\n", ret, portid);

    rte_eth_promiscuous_enable(portid);

    printf("Port %u, MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n\n",
        portid,
        ports_eth_addr[portid].addr_bytes[0],
        ports_eth_addr[portid].addr_bytes[1],
        ports_eth_addr[portid].addr_bytes[2],
        ports_eth_addr[portid].addr_bytes[3],
        ports_eth_addr[portid].addr_bytes[4],
        ports_eth_addr[portid].addr_bytes[5]);

    /* initialize port stats */
    memset(&port_statistics, 0, sizeof(port_statistics));
  }

  check_all_ports_link_status((uint8_t)nb_ports, enabled_port_mask);
}



void check_all_ports_link_status(
    uint8_t port_num,
    uint32_t port_mask) {

#define CHECK_INTERVAL 100 /* 100ms */
#define MAX_CHECK_TIME 90 /* 9s (90 * 100ms) in total */
  uint8_t portid, count, all_ports_up, print_flag = 0;
  struct rte_eth_link link;

  printf("\nChecking link status");
  fflush(stdout);

  for (count = 0; count <= MAX_CHECK_TIME; count++) {

    all_ports_up = 1;
    for (portid = 0; portid < port_num; portid++) {

      if ((port_mask & (1 << portid)) == 0)
        continue;

      memset(&link, 0, sizeof(link));
      rte_eth_link_get_nowait(portid, &link);

      if (print_flag == 1) {

        if (link.link_status)
          printf("Port %d Link Up - speed %u Mbps - %s\n",
              (uint8_t) portid,
              (unsigned) link.link_speed,
              (link.link_duplex == ETH_LINK_FULL_DUPLEX) ?
                  ("full-duplex") : ("half-duplex\n"));

        else
          printf("Port %d Link Down\n", (uint8_t) portid);

        continue;
      }

      /* clear all_ports_up flag if any link down */
      if (link.link_status == 0) {
        all_ports_up = 0;
        break;
      }

    }
    /* after finally printing all link status, get out */
    if (print_flag == 1)
      break;

    if (all_ports_up == 0) {
      printf(".");
      fflush(stdout);
      rte_delay_ms(CHECK_INTERVAL);
    }

    /* set the print_flag if all ports up or timeout */
    if (all_ports_up == 1 || count == (MAX_CHECK_TIME - 1)) {
      print_flag = 1;
      printf("done\n");
    }
  }
}

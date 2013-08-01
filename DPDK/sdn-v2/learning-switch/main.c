/*-
 *   BSD LICENSE
 * 
 *   Copyright(c) 2010-2013 Intel Corporation. All rights reserved.
 *   All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without 
 *   modification, are permitted provided that the following conditions 
 *   are met:
 * 
 *     * Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided with the 
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its 
 *       contributors may be used to endorse or promote products derived 
 *       from this software without specific prior written permission.
 * 
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


#include "main.h"
#include "setup.h"

#include "lib_pipeline/pipeline.h"


extern void send_burst(
    struct lcore_queue_conf *qconf,
    uint8_t port);


/* main processing loop */
void main_loop(void) {
  struct rte_mbuf *pkts_burst[MAX_PKT_BURST];
  struct rte_mbuf *m;

  pipeline_init();

  unsigned i, j, portid, nb_rx;

  // Return the ID of the execution unit we are running on.
  unsigned lcore_id = rte_lcore_id();
  struct lcore_queue_conf* qconf = &lcore_queue_conf[lcore_id];

  if (qconf->n_rx_queue == 0) {
    RTE_LOG(INFO, SWITCH, "lcore %u has nothing to do\n", lcore_id);
    while(1);
  }

  RTE_LOG(INFO, SWITCH, "Entering main loop on lcore %u\n", lcore_id);

  for (i = 0; i < qconf->n_rx_queue; i++) {
    portid = qconf->rx_queue_list[i];
    RTE_LOG(INFO, SWITCH, "Binded lcoreid %u <-> portid %u\n", lcore_id, portid);
  }


  uint64_t prev_tsc, cur_tsc, diff_tsc, timer_tsc;
  timer_tsc = 0;
  prev_tsc = 0;

  while (1) {

    cur_tsc = rte_rdtsc();

    /*
     * TX burst queue drain
     */
    diff_tsc = cur_tsc - prev_tsc;

    portid = qconf->rx_queue_list[0];

    if (unlikely(diff_tsc > BURST_TX_DRAIN)) {

      for (portid = 0; portid < MAX_PORTS; portid++) {

        if (qconf->tx_mbufs[portid].len == 0)
          continue;

        pipeline_send_burst(&lcore_queue_conf[lcore_id], (uint8_t) portid);
        qconf->tx_mbufs[portid].len = 0;
      }

      /* if timer is enabled */
      if (timer_period > 0) {

        /* advance the timer */
        timer_tsc += diff_tsc;

        /* if timer has reached its timeout */
        if (unlikely(timer_tsc >= (uint64_t) timer_period)) {

          /* do this only on master core */
          if (lcore_id == rte_get_master_lcore()) {
            //print_stats();
            /* reset the timer */
            timer_tsc = 0;
          }
        }
      }

      prev_tsc = cur_tsc;
    }

    /*
     * Read packet from RX queues
     */
    for (i = 0; i < qconf->n_rx_queue; i++) {

      portid = qconf->rx_queue_list[i];
      nb_rx = rte_eth_rx_burst((uint8_t) portid, 0,
             pkts_burst, MAX_PKT_BURST);

      port_statistics[portid].rx += nb_rx;

      for (j = 0; j < nb_rx; j++) {
        m = pkts_burst[j];
        rte_prefetch0(rte_pktmbuf_mtod(m, void *));
        start_pipeline(m, qconf);
      }
    }

  }
}



static int launch_one_lcore(
    __attribute__((unused)) void *dummy) {

  main_loop();
  return 0;
}



int MAIN(int argc, char **argv) {

  int ret;
  unsigned lcore_id;

  /* init EAL */
  ret = rte_eal_init(argc, argv);
  if (ret < 0)
    rte_exit(EXIT_FAILURE, "Invalid EAL arguments\n");

  argc -= ret;
  argv += ret;

  /* parse application arguments (after the EAL ones) */
  ret = parse_args(argc, argv);
  if (ret < 0)
    rte_exit(EXIT_FAILURE, "Invalid arguments\n");

  init_eal();
  init_ports();

  /* launch per-lcore init on every lcore */
  rte_eal_mp_remote_launch(launch_one_lcore, NULL, CALL_MASTER);
  RTE_LCORE_FOREACH_SLAVE(lcore_id) {
    if (rte_eal_wait_lcore(lcore_id) < 0)
      return -1;
  }

  return 0;
}


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

#include "io.h"


/* Send burst of packets on an output interface */
void send_burst(
    struct lcore_queue_conf *qconf,
    uint8_t port) {

  struct rte_mbuf **m_table;
  uint16_t n, queueid;
  int ret;

  queueid = qconf->tx_queue_id[port];
  m_table = (struct rte_mbuf **)qconf->tx_mbufs[port].m_table;
  n = qconf->tx_mbufs[port].len;

  ret = rte_eth_tx_burst(port, queueid, m_table, n);
  while (unlikely (ret < n)) {
    rte_pktmbuf_free(m_table[ret]);
    ret++;
  }

  qconf->tx_mbufs[port].len = 0;
}



inline struct rte_mbuf * pkt_duplicate(
    struct rte_mbuf *pkt,
    int use_clone) {

  struct rte_mbuf *hdr;

  /* Create new mbuf for the header. */
  if (unlikely ((hdr = rte_pktmbuf_alloc(header_pool)) == NULL))
    return (NULL);

  /* If requested, then make a new clone packet. */
  if (use_clone != 0 &&
      unlikely ((pkt = rte_pktmbuf_clone(pkt, clone_pool)) == NULL)) {
    rte_pktmbuf_free(hdr);
    return (NULL);
  }

  /* prepend new header */
  hdr->pkt.next = pkt;

  /* update header's fields */
  hdr->pkt.pkt_len = (uint16_t)(hdr->pkt.data_len + pkt->pkt.pkt_len);
  hdr->pkt.nb_segs = (uint8_t)(pkt->pkt.nb_segs + 1);

  /* copy metadata from source packet*/
  hdr->pkt.in_port = pkt->pkt.in_port;
  hdr->pkt.vlan_macip = pkt->pkt.vlan_macip;
  hdr->pkt.hash = pkt->pkt.hash;

  hdr->ol_flags = pkt->ol_flags;

  __rte_mbuf_sanity_check(hdr, RTE_MBUF_PKT, 1);
  return (hdr);
}



inline void pkt_send_single(
    struct rte_mbuf *pkt,
    struct lcore_queue_conf *qconf,
    uint8_t port) {

  uint16_t len = qconf->tx_mbufs[port].len;
  qconf->tx_mbufs[port].m_table[len] = pkt;
  qconf->tx_mbufs[port].len = ++len;

  /* Transmit packets */
  if (unlikely(MAX_PKT_BURST == len))
    send_burst(qconf, port);
}



void pkt_flood(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  struct rte_mbuf *mc;
  uint8_t port;

  RTE_MBUF_ASSERT(iphdr != NULL);

  /* Should we use rte_pktmbuf_clone() or not. */
  const uint32_t use_clone = 0;

  for (port = 0; port < 2; port++) {

      if (likely ((mc = pkt_duplicate(m, use_clone)) != NULL))
        pkt_send_single(mc, qconf, port);
      else if (use_clone == 0)
        rte_pktmbuf_free(m);
  }
}



int component_output(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  const uint32_t output_port = *( (uint32_t*) m->pkt.data );
  rte_pktmbuf_adj(m, sizeof(uint32_t));

  if (output_port == FLOOD_PORT) {
    RTE_LOG(INFO, SWITCH, "Flooding...\n");
    pkt_flood(m, qconf);
  } else if (output_port == DROP_PACKET) {
    rte_pktmbuf_free(m);
  } else {
    pkt_send_single(m, qconf, output_port);
  }

  return 0;
}

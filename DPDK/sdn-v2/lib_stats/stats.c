#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

#include "stats.h"
#include "globals.h"
#include "defs.h"

#include <getopt.h>

void print_stats(void) {
  uint64_t total_packets_dropped, total_packets_tx, total_packets_rx;
  unsigned portid;

  total_packets_dropped = 0;
  total_packets_tx = 0;
  total_packets_rx = 0;

  const char clr[] = { 27, '[', '2', 'J', '\0' };
  const char topLeft[] = { 27, '[', '1', ';', '1', 'H','\0' };

    /* Clear screen and move to top left */
  printf("%s%s", clr, topLeft);

  printf("\nPort statistics ====================================");

  for (portid = 0; portid < MAX_PORTS; portid++) {
    /* skip disabled ports */
    if ((enabled_port_mask & (1 << portid)) == 0)
      continue;
    printf("\nStatistics for port %u ------------------------------"
         "\nPackets sent: %24"PRIu64
         "\nPackets received: %20"PRIu64
         "\nPackets dropped: %21"PRIu64,
         portid,
         port_statistics[portid].tx,
         port_statistics[portid].rx,
         port_statistics[portid].dropped);

    total_packets_dropped += port_statistics[portid].dropped;
    total_packets_tx += port_statistics[portid].tx;
    total_packets_rx += port_statistics[portid].rx;
  }
  printf("\nAggregate statistics ==============================="
       "\nTotal packets sent: %18"PRIu64
       "\nTotal packets received: %14"PRIu64
       "\nTotal packets dropped: %15"PRIu64,
       total_packets_tx,
       total_packets_rx,
       total_packets_dropped);
  printf("\n====================================================\n");
}

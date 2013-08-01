
#include "globals.h"

#include <lib_module/module.h>
#include <lib_ethane/ethane.h>
#include <lib_io/io.hh>


extern "C" {}



dpdk::L2Switch l2_switch;
dpdk::Ethane   ethane;



extern "C" void pipeline_init() {
  ethane.component_ethane_init();
}



extern "C" void pipeline_send_burst(
    struct lcore_queue_conf *qconf,
    uint8_t port) {

  IO::send_burst(qconf, port);
}



extern "C" void start_pipeline(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  if(l2_switch.component_learn(m, qconf)) return;

  if(ethane.component_ethane_class(m, qconf)) return;
  if(ethane.component_ethane_action(m, qconf)) return;

  if(l2_switch.component_route(m, qconf)) return;
  if(IO::component_output(m, qconf)) return;
}


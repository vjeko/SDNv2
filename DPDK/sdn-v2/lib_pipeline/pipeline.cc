
#include "globals.h"

#include "lib_module/module.h"
#include "lib_ethane/ethane.h"

extern "C" {
#include "lib_io/io.h"
}

extern "C" void pipeline_init() {
  component_ethane_init();
}


extern "C" void start_pipeline(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf) {

  if(component_learn(m, qconf)) return;

  if(component_ethane_class(m, qconf)) return;
  if(component_ethane_action(m, qconf)) return;

  if(component_route(m, qconf)) return;
  if(component_output(m, qconf)) return;
}



#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <rte_mbuf.h>

void pipeline_send_burst(
    struct lcore_queue_conf *qconf,
    uint8_t port);

void pipeline_init(void);

void start_pipeline(
    struct rte_mbuf *m,
    struct lcore_queue_conf *qconf);


#endif /* PIPELINE_H_ */

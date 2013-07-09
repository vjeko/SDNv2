/*
 * config.h
 *
 *  Created on: Jul 7, 2013
 *      Author: vjeko
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdint.h>

/* Check the link status of all ports in up to 9s, and print them finally */
void check_all_ports_link_status(
    uint8_t port_num,
    uint32_t port_mask);

void init_eal(void);
void init_ports(void);

#endif /* CONFIG_H_ */

/*
 * parse.h
 *
 *  Created on: Jul 6, 2013
 *      Author: vjeko
 */

#ifndef PARSE_H_
#define PARSE_H_

void usage(const char *prgname);
int parse_args(int argc, char **argv);
int parse_portmask(const char *portmask);
int parse_timer_period(const char *q_arg);
unsigned int parse_nqueue(const char *q_arg);


#endif /* PARSE_H_ */

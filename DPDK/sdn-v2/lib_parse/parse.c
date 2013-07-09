#include <rte_config.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ethdev.h>

#include "parse.h"
#include "globals.h"
#include "defs.h"

#include <getopt.h>


/* display usage */
void usage(const char *prgname)
{
  printf("%s [EAL options] -- -p PORTMASK [-q NQ]\n"
         "  -p PORTMASK: hexadecimal bitmask of ports to configure\n"
         "  -q NQ: number of queue (=ports) per lcore (default is 1)\n"
       "  -T PERIOD: statistics will be refreshed each PERIOD seconds (0 to disable, 10 default, 86400 maximum)\n",
         prgname);
}


int parse_portmask(const char *portmask) {
  char *end = NULL;
  unsigned long pm;

  /* parse hexadecimal string */
  pm = strtoul(portmask, &end, 16);
  if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
    return -1;

  if (pm == 0)
    return -1;

  return pm;
}

int parse_timer_period(const char *q_arg)
{
  char *end = NULL;
  int n;

  /* parse number string */
  n = strtol(q_arg, &end, 10);
  if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
    return -1;
  if (n >= MAX_TIMER_PERIOD)
    return -1;

  return n;
}



unsigned int parse_nqueue(const char *q_arg) {
  char *end = NULL;
  unsigned long n;

  /* parse hexadecimal string */
  n = strtoul(q_arg, &end, 10);
  if ((q_arg[0] == '\0') || (end == NULL) || (*end != '\0'))
    return 0;
  if (n == 0)
    return 0;
  if (n >= MAX_RX_QUEUE_PER_LCORE)
    return 0;

  return n;
}


/* Parse the argument given in the command line of the application */
int parse_args(int argc, char **argv)
{
  int opt, ret;
  char **argvopt;
  int option_index;
  char *prgname = argv[0];
  static struct option lgopts[] = {
    {NULL, 0, 0, 0}
  };

  argvopt = argv;

  while ((opt = getopt_long(argc, argvopt, "p:q:T:",
          lgopts, &option_index)) != EOF) {

    switch (opt) {
    /* portmask */
    case 'p':
      enabled_port_mask = parse_portmask(optarg);
      if (enabled_port_mask == 0) {
        printf("invalid portmask\n");
        usage(prgname);
        return -1;
      }
      break;

    /* nqueue */
    case 'q':
      rx_queue_per_lcore = parse_nqueue(optarg);
      if (rx_queue_per_lcore == 0) {
        printf("invalid queue number\n");
        usage(prgname);
        return -1;
      }
      break;

    /* timer period */
    case 'T':
      timer_period = parse_timer_period(optarg) * 1000 * TIMER_MILLISECOND;
      if (timer_period < 0) {
        printf("invalid timer period\n");
        usage(prgname);
        return -1;
      }
      break;

    /* long options */
    case 0:
      usage(prgname);
      return -1;

    default:
      usage(prgname);
      return -1;
    }
  }

  if (optind >= 0)
    argv[optind-1] = prgname;

  ret = optind-1;
  optind = 0; /* reset getopt lib */
  return ret;
}

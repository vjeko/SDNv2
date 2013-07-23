/*
 * rule.hh
 *
 *  Created on: Jul 23, 2013
 *      Author: vjeko
 */

#ifndef RULE_HH_
#define RULE_HH_


namespace dpdk {

struct Rule {

  void insert(size_t rule_nb) {
    index_set_.insert(rule_nb);
  }

  std::set<size_t> index_set_;

};

}

#endif /* RULE_HH_ */

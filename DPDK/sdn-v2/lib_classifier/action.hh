/*
 * action.hh
 *
 *  Created on: Jul 23, 2013
 *      Author: vjeko
 */

#ifndef ACTION_HH_
#define ACTION_HH_

namespace dpdk {

struct Action {

  size_t action_type_;
  void (*continuation)(void);

};

}

#endif /* ACTION_HH_ */

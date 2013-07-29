/*
 * match.hh
 *
 *  Created on: Jul 28, 2013
 *      Author: vjeko
 */

#ifndef MATCH_HH_
#define MATCH_HH_


#include <bitset>

namespace dpdk {

const size_t MAX_TABLE_SIZE = 100000;
typedef std::bitset<MAX_TABLE_SIZE> BitSetT;

template<unsigned int Size, unsigned int Offset>
struct Match {
  static const unsigned int SizeT = Size;
  static const unsigned int OffsetT = Offset;
};

}

#endif /* MATCH_HH_ */

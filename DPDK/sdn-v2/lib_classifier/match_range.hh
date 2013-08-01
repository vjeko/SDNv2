/*
 * range_match.hh
 *
 *  Created on: Jul 28, 2013
 *      Author: vjeko
 */

#ifndef MATCH_RANGE_HH_
#define MATCH_RANGE_HH_


#include <boost/integer.hpp>

#include <boost/icl/interval_set.hpp>
#include <boost/icl/separate_interval_set.hpp>
#include <boost/icl/split_interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>

#include "match.hh"

namespace dpdk {

using std::array;
using std::pair;
using std::unordered_map;


using namespace boost::icl;


template<typename Match>
struct MatchRange {

  typedef typename boost::uint_t< Match::SizeT * 8 >::least   IntegralType;
  typedef pair<IntegralType, IntegralType>                    KeyType;


  MatchRange() {};


  void add_rule(size_t rule_nb, KeyType range) {

    interval_.insert( make_pair(
        interval<IntegralType>::closed(range.first, range.second), rule_nb) );

  }



  bool find(BitSetT& my_bitset, BitSetT& bitset, void* packet) {

    IntegralType field;

    unsigned char* bytePtr = reinterpret_cast<unsigned char*>(packet);
    bytePtr += Match::OffsetT;

    memcpy(&field, bytePtr, Match::SizeT);
    my_bitset.reset();

    printf("%u\n", field);

    auto it = interval_.find(field);
    if (it == interval_.end()) {
      bitset &= my_bitset;
      return false;
    }

    for(; it != interval_.end(); ++it) {
      my_bitset.set(it->second, true);
    }

    bitset &= my_bitset;
    return true;

  }


  interval_map<IntegralType, size_t> interval_;

};


}

#endif /* RANGE_MATCH_HH_ */

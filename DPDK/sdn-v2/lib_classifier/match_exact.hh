/*
 * exact_match.hh
 *
 *  Created on: Jul 28, 2013
 *      Author: vjeko
 */

#ifndef MATCH_EXACT_HH_
#define MATCH_EXACT_HH_

#include <boost/array.hpp>
#include <boost/functional/hash/extensions.hpp>

#include <unordered_map>
#include <iostream>
#include <array>
#include <bitset>

namespace dpdk {

using std::array;
using std::pair;
using std::unordered_map;



template<typename Match>
struct MatchExact {

  typedef array<uint8_t, Match::SizeT> KeyType;
  typedef boost::hash< array<uint8_t, Match::SizeT> > HashType;

  MatchExact() {};


  void add_rule(size_t rule_nb, KeyType field) {

    auto src_it = map_.find(field);
    if (src_it == map_.end()) {
      auto pair = map_.insert( std::make_pair(field, Rule()) );
      src_it = pair.first;
    }

    src_it->second.insert(rule_nb);
  }



  bool find(BitSetT& my_bitset, BitSetT& bitset, void* packet) {

    KeyType field;

    unsigned char* bytePtr = reinterpret_cast<unsigned char*>(packet);
    bytePtr += Match::OffsetT;

    uint8_t* packet_field = ((uint8_t*) bytePtr);
    uint8_t* array_field = &std::get<0>(field);

    uint32_t& a =  * (uint32_t*) array_field;
    uint32_t& b =  * (uint32_t*) packet_field;

    memcpy(array_field, packet_field, 4);
    my_bitset.reset();

    auto it = map_.find(field);
    if (it == map_.end()) {
      bitset &= my_bitset;
      return false;
    }

    auto bit_it = it->second.index_set_.begin();
    for(; bit_it != it->second.index_set_.end(); ++bit_it) {
      my_bitset.set(*bit_it, true);
    }

    bitset &= my_bitset;
    return true;
  }


  unordered_map<KeyType, Rule, HashType> map_;

};

}

#endif /* EXACT_MATCH_HH_ */

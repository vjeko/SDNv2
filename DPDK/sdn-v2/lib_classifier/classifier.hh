/*
 * Classifier.h
 *
 *  Created on: Jul 15, 2013
 *      Author: vjeko
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#define BOOST_ALL_DYN_LINK

#include <boost/dynamic_bitset.hpp>
#include <boost/array.hpp>
#include <boost/functional/hash/extensions.hpp>

#include <unordered_map>
#include <set>
#include <iostream>
#include <array>
#include <bitset>

#include "action.hh"
#include "rule.hh"

namespace dpdk {

using std::array;



const size_t MAX_TABLE_SIZE = 100000;
typedef std::bitset<MAX_TABLE_SIZE> BitSetT;



template<unsigned int... Types>
struct Classifier {};



template<unsigned int Size, unsigned int Offset, unsigned int... Tail>
struct Classifier<Size, Offset, Tail...> {

  typedef array<uint8_t, Size> ValueType;
  typedef boost::hash< array<uint8_t, Size> > HashType;

  Classifier() :
    rule_counter_(next_.rule_counter_),
    action_table_(next_.action_table_){}



  template<typename... Others>
  Action* add_rule(Others... others...) {
    //BOOST_LOG_TRIVIAL(trace) << "Adding rule #" << rule_counter_;
    _add_rule(rule_counter_, others...);
    rule_counter_++;
    return &action_table_[rule_counter_ - 1];
  }



  template<typename... Others>
  void _add_rule(size_t rule_nb, ValueType field, Others... others...) {

    next_._add_rule(rule_nb, others...);

    auto src_it = map_.find(field);
    if (src_it == map_.end()) {
      auto pair = map_.insert( std::make_pair(field, Rule()) );
      src_it = pair.first;
    }

    src_it->second.insert(rule_nb);
  }



  template<typename... Others>
  void _find(BitSetT& bitset, void* packet) {

    ValueType field;
    uint8_t* packet_field = (uint8_t*) packet + Offset;
    uint8_t* array_field = &std::get<0>(field);

    memcpy(array_field, packet_field, 4);
    BitSetT my_bitset(rule_counter_);
    my_bitset.reset();

    auto it = map_.find(field);
    if (it == map_.end()) {
      bitset &= my_bitset;
      return;
    }

    auto bit_it = it->second.index_set_.begin();
    for(; bit_it != it->second.index_set_.end(); ++bit_it) {
      my_bitset.set(*bit_it, true);
    }

    next_._find(bitset, packet);
    bitset &= my_bitset;
  }



  template<typename... Others>
  Action* find(void* packet) {
    BitSetT bitset(rule_counter_);
    bitset.reset().flip();
    _find(bitset, packet);

    for(auto i = 0; i < rule_counter_; i++) {
      if(bitset[i]) {
        return &action_table_[i];
      }
    }

    return NULL;
  }



  template<typename... Others>
  void find_rule(Others... others...) {

    BitSetT bitset(rule_counter_);
    _find_rule(bitset, others...);

  }



  template<typename... Others>
  void _find_rule(BitSetT& bitset, ValueType field, Others... others...) {

    BitSetT my_bitset(rule_counter_);
    my_bitset.reset();

    auto it = map_.find(field);
    if (it == map_.end()) {
      return;
    }

    auto bit_it = it->second.index_set_.begin();
    for(; bit_it != it->second.index_set_.end(); ++bit_it) {
      my_bitset.set(*bit_it, true);
    }

    next_._find_rule(bitset, others...);
    bitset &= my_bitset;
  }



  Classifier<Tail ...> next_;

  size_t& rule_counter_;
  std::unordered_map<size_t, Action>& action_table_;
  std::unordered_map<ValueType, Rule, HashType> map_;

};



template<unsigned int Size, unsigned int Offset>
struct Classifier<Size, Offset> {

  typedef array<uint8_t, Size> ValueType;
  typedef boost::hash< array<uint8_t, Size> > HashType;

  Classifier() : rule_counter_(0) {}

  Action* add_rule(ValueType field) {
    //BOOST_LOG_TRIVIAL(trace) << "Adding rule #" << rule_counter_;
    _add_rule(rule_counter_, field);
    rule_counter_++;
    return action_table_[rule_counter_ - 1];
  }



  void _add_rule(size_t rule_nb, ValueType field) {

    auto it = map_.find(field);
    if (it == map_.end()) {
      auto pair = map_.insert( std::make_pair(field, Rule()) );
      it = pair.first;
    }

    it->second.insert(rule_nb);
  }



  void _find_rule(BitSetT& bitset, ValueType field) {
    BitSetT my_bitset(rule_counter_);

    auto it = map_.find(field);
    if (it == map_.end()) return;

    auto bit_it = it->second.index_set_.begin();
    for(; bit_it != it->second.index_set_.end(); ++bit_it) {
      my_bitset.set(*bit_it, true);
    }

    (bitset) &= (my_bitset);
  }



  void find_rule(ValueType field) {
    BitSetT bitset(rule_counter_);
    _find_rule(bitset, field);
  }




  template<typename... Others>
  void _find(BitSetT& bitset, void* packet) {

    ValueType field;
    uint8_t* packet_field = (uint8_t*) packet + Offset;
    uint8_t* array_field = &std::get<0>(field);

    memcpy(array_field, packet_field, 4);
    BitSetT my_bitset(rule_counter_);
    my_bitset.reset();

    auto it = map_.find(field);
    if (it == map_.end()) {
      bitset &= my_bitset;
      return;
    }

    auto bit_it = it->second.index_set_.begin();
    for(; bit_it != it->second.index_set_.end(); ++bit_it) {
      my_bitset.set(*bit_it, true);
    }

    bitset &= my_bitset;
  }



  template<typename... Others>
  void find(void* packet) {
    BitSetT bitset(rule_counter_);
    bitset.reset().flip();
    _find(bitset, packet);
  }




  size_t rule_counter_;
  std::unordered_map<size_t, Action> action_table_;
  std::unordered_map<ValueType, Rule, HashType> map_;

};


} /* namespace dpdk */
#endif /* CLASSIFIER_H_ */

/*
 * Classifier.h
 *
 *  Created on: Jul 15, 2013
 *      Author: vjeko
 */

#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#define BOOST_ALL_DYN_LINK

#include <set>

#include "action.hh"
#include "rule.hh"
#include "match.hh"
#include "match_exact.hh"
#include "match_range.hh"

namespace dpdk {

using std::unordered_map;



template<class... >
struct Classifier {};


template<class Type, class... Tail>
struct Classifier<Type, Tail...> {


  Classifier() :
    rule_counter_(next_.rule_counter_),
    action_table_(next_.action_table_) {}



  template<typename... Others>
  inline Action* add_rule(Others... others...) {
    //BOOST_LOG_TRIVIAL(trace) << "Adding rule #" << rule_counter_;

    _add_rule(rule_counter_, others...);
    rule_counter_++;
    return &action_table_[rule_counter_ - 1];
  }



  template<typename... Others>
  inline void _add_rule(
      size_t rule_nb,
      typename Type::KeyType field,
      Others... others...) {

    next_._add_rule(rule_nb, others...);
    impl_.add_rule(rule_nb, field);

  }



  template<typename... Others>
  inline void _find(BitSetT& bitset, void* packet) {

    BitSetT my_bitset(rule_counter_);

    bool found = impl_.find(my_bitset, bitset, packet);
    if(!found) return;

    next_._find(bitset, packet);
    bitset &= my_bitset;
  }



  template<typename... Others>
  inline Action* find(void* packet) {
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



  Type                  impl_;
  Classifier<Tail ...>  next_;

  size_t&                                     rule_counter_;
  unordered_map<size_t, Action>&              action_table_;
};



template<class Type>
struct Classifier<Type> {

  Classifier() : rule_counter_(0) {}


  Action* add_rule(typename Type::KeyType field) {
    //BOOST_LOG_TRIVIAL(trace) << "Adding rule #" << rule_counter_;
    _add_rule(rule_counter_, field);
    rule_counter_++;
    return action_table_[rule_counter_ - 1];
  }



  void _add_rule(size_t rule_nb, typename Type::KeyType field) {
    impl_.add_rule(rule_nb, field);
  }



  template<typename... Others>
  void _find(BitSetT& bitset, void* packet) {

    BitSetT my_bitset(rule_counter_);

    bool found = impl_.find(my_bitset, bitset, packet);
    if(!found) return;

    bitset &= my_bitset;
  }



  template<typename... Others>
  void find(void* packet) {
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


  Type                  impl_;

  size_t                                     rule_counter_;
  unordered_map<size_t, Action>              action_table_;

};


} /* namespace dpdk */
#endif /* CLASSIFIER_H_ */

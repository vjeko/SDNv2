
#include "classifier.hh"

#include <iostream>

#define BOOST_ALL_DYN_LINK

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <iostream>
#include <chrono>
#include <thread>


#include <boost/icl/interval_set.hpp>
#include <boost/icl/separate_interval_set.hpp>
#include <boost/icl/split_interval_set.hpp>
#include <boost/icl/split_interval_map.hpp>


namespace logging = boost::log;

using std::chrono::high_resolution_clock;
using std::chrono::nanoseconds;
using std::array;

void example_hook() {
  std::cout << "Just an example hook." << std::endl;
}

int main() {

  uint32_t packet[4];

  using dpdk::MatchExact;
  using dpdk::MatchRange;

  typedef dpdk::Match<sizeof(uint32_t), 0>                      Match0;
  typedef dpdk::Match<sizeof(uint32_t), sizeof(uint32_t)>       Match1;
  typedef dpdk::Match<sizeof(uint32_t), sizeof(uint32_t) * 2>   Match2;
  typedef dpdk::Match<sizeof(uint32_t), sizeof(uint32_t) * 3>   Match3;

  typedef dpdk::Classifier<
      MatchRange<Match0>,
      MatchExact<Match1>,
      MatchExact<Match2>,
      MatchExact<Match3>
      > CustomFoo;

  union TUnion {
    array<uint8_t, sizeof(uint32_t)> array_;
    uint32_t int_;
  };


  MatchRange<Match0>::KeyType f1;
  TUnion f2;
  TUnion f3;
  TUnion f4;


  CustomFoo f;

  auto start = high_resolution_clock::now();


  for (auto i = 0; i < 20; i++) {
    f1.first = 1 + i;
    f1.second = 100 + i;

    f2.int_ = 2 + i;
    f3.int_ = 3 + i;
    f4.int_ = 4 + i;

    dpdk::Action* a = f.add_rule(f1, f2.array_, f3.array_, f4.array_);
    a->type_ = 999;
  }


  auto finish = high_resolution_clock::now();
  auto delta = std::chrono::duration_cast<nanoseconds>(finish - start);
  std::cout << "Insertion took " << delta.count() << " ns" << std::endl;;

  packet[0] = 20000;
  packet[1] = 3;
  packet[2] = 4;
  packet[3] = 5;


  start = std::chrono::high_resolution_clock::now();

  dpdk::Action* a = f.find((void*) &packet);


  finish = std::chrono::high_resolution_clock::now();
  delta = std::chrono::duration_cast<nanoseconds>(finish - start);
  std::cout << "Lookup took " << delta.count() << " ns" << std::endl;

  if (a)
    std::cout << "Action type: " << a->type_ << std::endl;


}

cmake_minimum_required(VERSION 2.8)

include_directories ($ENV{RTE_SDK}/lib/librte_mbuf)
include_directories ($ENV{RTE_SDK}/lib/librte_ether)
include_directories ($ENV{RTE_SDK}/lib/librte_mempool)
include_directories ($ENV{RTE_SDK}/lib/librte_ring)
include_directories ($ENV{RTE_SDK}/lib/librte_eal/common/include/)
include_directories ($ENV{RTE_SDK}/lib/librte_eal/linuxapp/eal/include)
include_directories ($ENV{RTE_SDK}/lib/librte_eal/common/include/x86_64)
include_directories ($ENV{RTE_SDK}/x86_64-default-linuxapp-gcc/include)
include_directories ($ENV{RTE_SDK}/sdn-v2/learning-switch)

add_library(
  ethane
  SHARED
  ethane.cc
)

cmake_minimum_required(VERSION 2.8)

list( APPEND CMAKE_CXX_FLAGS "-std=gnu++11")

include_directories (../../..//lib/librte_mbuf)
include_directories (../../../lib/librte_ether)
include_directories (../../..//lib/librte_mempool)
include_directories (../../..//lib/librte_ring)
include_directories (../../..//lib/librte_eal/common/include/)
include_directories (../../..//lib/librte_eal/linuxapp/eal/include)
include_directories (../../..//lib/librte_eal/common/include/x86_64)
include_directories (../../../x86_64-default-linuxapp-gcc/include)

add_library(
  module
  SHARED
  module.cc
)

cmake_minimum_required(VERSION 2.8)
project(sdn)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ALL_LIBS 
  $ENV{RTE_SDK}/sdn-v2
  $ENV{RTE_SDK}/sdn-v2/lib_io 
  $ENV{RTE_SDK}/sdn-v2/lib_parse
  $ENV{RTE_SDK}/sdn-v2/lib_setup
  $ENV{RTE_SDK}/sdn-v2/lib_stats
  $ENV{RTE_SDK}/sdn-v2/lib_l2
  $ENV{RTE_SDK}/sdn-v2/lib_ethane
  $ENV{RTE_SDK}/sdn-v2/lib_pipeline
  $ENV{RTE_SDK}/sdn-v2/learning-switch
  )

set(EXTRA_CFLAGS "")
set(EXTRA_LDFLAGS "")

set(CMAKE_C_FLAGS "-Wall -Wno-long-long")

foreach(PATH ${ALL_LIBS})
  set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -I${PATH}")
  set(EXTRA_LDFLAGS "${EXTRA_LDFLAGS} -L${PATH}")
  set(EXTRA_LDFLAGS "${EXTRA_LDFLAGS} -rpath ${PATH}")
endforeach(PATH)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fPIC") 
set(EXTRA_LDFLAGS "${EXTRA_LDFLAGS} -lio -lparse -lsetup -ll2 -lstats -lethane -lpipeline")
set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -I$ENV{RTE_SDK}/x86_64-default-linuxapp-gcc/include")

add_subdirectory(lib_io)
add_subdirectory(lib_parse)
add_subdirectory(lib_setup)
add_subdirectory(lib_l2)
add_subdirectory(lib_ethane)
add_subdirectory(lib_stats)
add_subdirectory(lib_pipeline)

ADD_CUSTOM_TARGET(learning-switch
  export EXTRA_LDFLAGS=${EXTRA_LDFLAGS} && 
  export EXTRA_CFLAGS=${EXTRA_CFLAGS} && 
  cd $ENV{RTE_SDK}/sdn-v2/learning-switch && make
  VERBATIM)

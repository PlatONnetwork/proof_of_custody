cmake_minimum_required(VERSION 3.10)
project(mp-spdz)

ADD_DEFINITIONS(-DPREP_DIR="Player-Data/")
# ADD_DEFINITIONS(-DUSE_NTL=0)
ADD_DEFINITIONS(-DUSE_GF2N_LONG=1)

option(VERBOSE OFF "")
IF(VERBOSE)
  # ADD_DEFINITIONS(-DDEBUG=1)
  # ADD_DEFINITIONS(-DVERBOSE_OPTIONS=1)
  # ADD_DEFINITIONS(-DDEBUG_THREADS=1)
  ADD_DEFINITIONS(-DVERBOSE=1)
  # ADD_DEFINITIONS(-DOTEXT_DEBUG=1)
ENDIF()
IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
  ADD_DEFINITIONS(-DDEBUG=1)
  ADD_DEFINITIONS(-DVERBOSE_OPTIONS=1)
  ADD_DEFINITIONS(-DDEBUG_THREADS=1)
  ADD_DEFINITIONS(-DVERBOSE=1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
ELSE()
  ADD_DEFINITIONS(-DNDEBUG=1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF()

option(INSECURE OFF "")
IF(INSECURE)
  ADD_DEFINITIONS(-DINSECURE=1)
ENDIF()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath -Wl,./local/lib -L./local/lib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/local/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/local/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/SimpleOT)

file(GLOB_RECURSE math_src Math/*.cpp)
file(GLOB_RECURSE tools_src Tools/*.cpp)
file(GLOB_RECURSE network_src Networking/*.cpp)
set(comm_src ${math_src} ${tools_src} ${network_src})
file(GLOB_RECURSE processor_src Processor/*.cpp)
file(GLOB_RECURSE ot_src OT/*.cpp)
file(GLOB_RECURSE gc_src GC/square64.cpp GC/Instruction.cpp)

link_libraries(pthread mpir mpirxx sodium ssl crypto boost_system rt simpleot)

add_library(spdz ${spdz_src} ${comm_src} ${processor_src} ${utils_src} ${ot_src} ${gc_src})

# about mascot simple example
add_executable(mascot_example.x Source/mascot_example.cpp)
target_link_libraries(mascot_example.x spdz)

# about POC
file(GLOB_RECURSE poc_src POC/*.cpp)
add_library(poc ${poc_src})
target_link_libraries(poc spdz mcl mclbn384_256)

function(compile_poc_programs prog_name)
  add_executable(${prog_name}.x Source/${prog_name}.cpp)
  target_link_libraries(${prog_name}.x poc)
endfunction()
compile_poc_programs(poc_online_op_test)
compile_poc_programs(poc_main)
compile_poc_programs(poc_main2fork)


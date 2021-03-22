cmake_minimum_required(VERSION 2.8)
project(mp-spdz)


add_definitions(-DDEBUG=1)
add_definitions(-DPREP_DIR="Player-Data/")
add_definitions(-DUSE_NTL=0)
add_definitions(-DUSE_GF2N_LONG=1)
add_definitions(-DVERBOSE_OPTIONS=1)
add_definitions(-DDEBUG_THREADS=1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I./local/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath -Wl,./local/lib -L./local/lib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unused-parameter")

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


# about POC
set(poc_src POC/mascot-example.cpp)
add_executable(mascot-example.x ${poc_src})
target_link_libraries(mascot-example.x spdz)



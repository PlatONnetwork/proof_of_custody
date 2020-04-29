cmake_minimum_required(VERSION 3.1...3.14)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

project(PROOF_OF_CUSTODY VERSION 1.0 DESCRIPTION "POC" LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)

include_directories($<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

add_definitions(-DMAX_MOD_SZ=7)
add_compile_options(
        -maes 
        -mpclmul
        -msse4.1
        -mavx
        -march=core2
)

find_package(MPIR REQUIRED)
find_package(MPIRXX REQUIRED)
find_package(OpenSSL REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
ELSE()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF()


# poc
file(GLOB_RECURSE poc_srcs src/*.cpp)
add_library(poc STATIC ${poc_srcs})

target_link_libraries(poc PRIVATE
        Threads::Threads OpenSSL::Crypto OpenSSL::SSL
        ${MPIR_LIBRARIES} ${MPIRXX_LIBRARIES}
        mcl
        mclbn384_256
        )

add_executable(main.x main.cpp)
target_link_libraries(main.x PRIVATE poc)

add_executable(mainfork mainfork.cpp)
target_link_libraries(mainfork PRIVATE poc)

add_executable(Setup.x Setup.cpp)
target_link_libraries(Setup.x PRIVATE OpenSSL::Crypto OpenSSL::SSL poc)

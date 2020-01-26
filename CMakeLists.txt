cmake_minimum_required(VERSION 3.1...3.14)

if (${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif ()

project(SCALE_MAMBA VERSION 1.0 DESCRIPTION "SCALE MAMBA" LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
set(CMAKE_CXX_STANDARD 11)

include_directories($<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# TODO is it possible to make this into a runtime parameter?
# TODO why is it not in config.h as well?
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
find_package(CryptoPP REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


file(GLOB exceptions Exceptions/*.cpp)
file(GLOB io Input_Output/*.cpp)
file(GLOB lsss LSSS/*.cpp)
file(GLOB mpcmath Math/*.cpp)
file(GLOB offline Offline/*.cpp)
file(GLOB online Online/*.cpp)
file(GLOB processor Processor/*.cpp)
file(GLOB system System/*.cpp)
file(GLOB tools Tools/*.cpp)

# scale
add_library(scale STATIC
        ${exceptions}
        ${io}
        ${lsss}
        ${mpcmath}
        ${offline}
        ${online}
        ${ot}
        ${processor}
        ${system}
        ${tools}
        )

target_link_libraries(scale PRIVATE
        Threads::Threads OpenSSL::Crypto OpenSSL::SSL
        ${CRYPTOPP_LIBRARIES} ${MPIR_LIBRARIES} ${MPIRXX_LIBRARIES}
        mcl
        mclbn384_256
        )

add_executable(Player.x Player.cpp)
target_link_libraries(Player.x PRIVATE scale)

add_executable(Setup.x Setup.cpp)
target_link_libraries(Setup.x PRIVATE OpenSSL::Crypto OpenSSL::SSL scale)

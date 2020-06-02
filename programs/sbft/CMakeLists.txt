set(server_source_files server.cpp utils.cpp)

set(concord_path ${CMAKE_CURRENT_SOURCE_DIR}/../../3rd/concord-bft)
#set(concord_path ${CMAKE_CURRENT_SOURCE_DIR}/../concord-bft-demo-master/deps/concord-bft)
set(concord_build_path ${concord_path}/build)

#
# will be optimize in the future
#

add_executable(server ${server_source_files} ${concord_path}/tools/KeyfileIOUtils.cpp ${concord_path}/logging/src/Logger.cpp)
set_property(SOURCE ${concord_path}/logging/src/Logger.cpp APPEND PROPERTY COMPILE_DEFINITIONS DEFAULT_LOGGER_NAME="CONCORD")


target_include_directories(server PUBLIC ${concord_path}/bftengine/include
        ${concord_path}/bftengine/include/simplestatetransfer
        ${concord_path}/bftengine/include/bftengine
        ${concord_path}/bftengine/include/bcstatetransfer
        ${concord_path}/communication/include
        ${concord_path}/logging/include
        ${concord_path}/util/include
        ${concord_path}/util
        ${concord_path}/tools
        ${concord_path}/threshsign/include
        )

target_link_directories(server PUBLIC ${concord_path}/build/bftengine
        ${concord_build_path}/lib
        ${concord_build_path}/util
        ${concord_build_path}/logging
        ${concord_build_path}/threshsign
        ${concord_build_path}/storage
        ${concord_build_path}/bftengine/src/preprocessor
        ${concord_build_path}/communication
        )

target_link_libraries(server PUBLIC corebft threshsign concordbft_storage preprocessor 
bftcommunication util cryptopp relic crypto  boost_system ssl pthread)


#################################

set(client_source_files client.cpp utils.cpp)

add_executable(client ${client_source_files} ${concord_path}/tools/KeyfileIOUtils.cpp ${concord_path}/logging/src/Logger.cpp)
set_property(SOURCE ${concord_path}/logging/src/Logger.cpp APPEND PROPERTY COMPILE_DEFINITIONS DEFAULT_LOGGER_NAME="CONCORD")


target_include_directories(client PUBLIC ${concord_path}/bftengine/include
        ${concord_path}/bftengine/include/simplestatetransfer
        ${concord_path}/bftengine/include/bftengine
        ${concord_path}/bftengine/include/bcstatetransfer
        ${concord_path}/communication/include
        ${concord_path}/logging/include
        ${concord_path}/util/include
        ${concord_path}/util
        ${concord_path}/tools
        ${concord_path}/threshsign/include
        )

target_link_directories(client PUBLIC ${concord_path}/build/bftengine
        ${concord_build_path}/lib
        ${concord_build_path}/util
        ${concord_build_path}/logging
        ${concord_build_path}/threshsign
        ${concord_build_path}/storage
        ${concord_build_path}/bftengine/src/preprocessor
        ${concord_build_path}/communication
        )

target_link_libraries(client PUBLIC corebft threshsign concordbft_storage preprocessor bftcommunication util 
cryptopp relic crypto  boost_system ssl pthread)


#################################


add_executable(mainfork-sbft mainfork-sbft.cpp mainfork-sbft-client.cpp utils.cpp
${concord_path}/tools/KeyfileIOUtils.cpp ${concord_path}/logging/src/Logger.cpp)
set_property(SOURCE ${concord_path}/logging/src/Logger.cpp APPEND PROPERTY COMPILE_DEFINITIONS DEFAULT_LOGGER_NAME="CONCORD")


target_include_directories(mainfork-sbft PUBLIC ${concord_path}/bftengine/include
        ${concord_path}/bftengine/include/simplestatetransfer
        ${concord_path}/bftengine/include/bftengine
        ${concord_path}/bftengine/include/bcstatetransfer
        ${concord_path}/communication/include
        ${concord_path}/logging/include
        ${concord_path}/util/include
        ${concord_path}/util
        ${concord_path}/tools
        ${concord_path}/threshsign/include
        )

target_link_directories(mainfork-sbft PUBLIC ${concord_path}/build/bftengine
        ${concord_build_path}/lib
        ${concord_build_path}/util
        ${concord_build_path}/logging
        ${concord_build_path}/threshsign
        ${concord_build_path}/storage
        ${concord_build_path}/bftengine/src/preprocessor
        ${concord_build_path}/communication
        )

target_link_libraries(mainfork-sbft PUBLIC corebft threshsign 
concordbft_storage preprocessor bftcommunication util cryptopp relic crypto  boost_system ssl pthread
poc
)




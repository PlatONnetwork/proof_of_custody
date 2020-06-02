

include_directories(.)

add_executable(Setup.x Setup.cpp)
target_link_libraries(Setup.x PRIVATE OpenSSL::Crypto OpenSSL::SSL poc)
        
add_executable(main.x main.cpp)
target_link_libraries(main.x PRIVATE poc)


add_executable(mainfork mainfork.cpp)
target_link_libraries(mainfork PRIVATE poc)


# Concord-BFT, base on SBFT
add_subdirectory(sbft)










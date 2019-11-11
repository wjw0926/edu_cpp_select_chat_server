cmake_minimum_required(VERSION 3.10)
project(edu_cpp_select_chat_server)

set(CMAKE_CXX_STANDARD 17)

# Reference: https://gist.github.com/lefticus/10191322#c-coding-standards-part-0-automated-code-analysis
# -Wall: basic warning option
# -Wextra: extra warning option
# -Wshadow: warn the user if a variable declaration shadows another with the same name in the same scope
# -Wnon-virtual-dtor: warn the user if a class with virtual functions has a non-virtual destructor; This can lead to hard to track down memory errors
# -pedantic: warn about non-portable code, C++ that uses language extensions
add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)

add_subdirectory(network)

set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-build-debug)

find_package(hiredis REQUIRED)
include_directories( ${HIREDIS_INCLUDE_DIRS} )

add_executable(select_chat_server
        main.cpp
        server/packet.hpp
        server/config.hpp
        server/chat_server.cpp
        server/chat_server.hpp
        server/user.hpp
        server/user_manager.cpp
        server/user_manager.hpp
        server/room.cpp
        server/room.hpp
        server/packet_manager.cpp
        server/packet_manager.hpp
        server/redis_manager.cpp
        server/redis_manager.hpp
        server/include/CRedisConn.h)

target_link_libraries(select_chat_server
        network
        ${HIREDIS_LIBRARIES})

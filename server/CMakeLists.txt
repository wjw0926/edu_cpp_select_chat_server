cmake_minimum_required(VERSION 3.10)
project(edu_cpp_select_chat_server)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)

include_directories(include)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
find_package(hiredis REQUIRED)
include_directories(${HIREDIS_INCLUDE_DIRS})

add_library(server STATIC
        packet.hpp
        error.hpp
        config.hpp
        chat_server.cpp
        chat_server.hpp
        user.hpp
        user_manager.cpp
        user_manager.hpp
        room.hpp
        room_manager.cpp
        room_manager.hpp
        packet_manager.cpp
        packet_manager.hpp
        redis_manager.cpp
        redis_manager.hpp)

TARGET_LINK_LIBRARIES(server
        network
        ${HIREDIS_LIBRARIES})

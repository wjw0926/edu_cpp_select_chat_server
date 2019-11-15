cmake_minimum_required(VERSION 3.10)
project(edu_cpp_select_chat_server)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic)

add_library(network STATIC
        error.hpp
        config.hpp
        tcp_socket.cpp
        tcp_socket.hpp
        server_socket.cpp
        server_socket.hpp
        tcp_server.cpp
        tcp_server.hpp
        session.cpp
        session.hpp)

# edu_cpp_select_chat_server

Overview
--------
This project contains a C++ source of select chat server.

Components
--------
##### Network library
* <code>network/</code> contains source of I/O multiplexing TCP server using select system call.
##### Chat server logic library
* <code>server/</code> contains source of chat server with room enter/leave/chat features.
* Chat server uses TCP network library.
* Login feature is implemented using redis client.
##### Application
* <code>main.cpp</code> which represents application layer runs chat server using network and chat server logic library.

Naming Rules
--------
##### File name
1. All C++ source files use <code>.cpp</code> for their extension.
2. All C++ header files use <code>.hpp</code> for their extension.
3. All C++ source/header files use <code>snake_case</code> for their name.

##### Class
1. Classes are defined in C++ header files.
2. Class name is <code>PascalCase</code> of the header file name where it is defined.
> ex) class TCPSocket is defined in tcp_socket.hpp

##### Member
1. All member functions use <code>PascalCase</code> for their name.
2. Parameter of member functions use <code>snake_case</code> for their name.
3. All member variables use <code>snake_case</code> for their name.
4. All private member variables use trailing underscore <code>_</code> at the end of their name.

Author
--------
[Jaewook Woo](https://github.com/wjw0926)
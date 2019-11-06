//
// Created by jwwoo on 2019-11-01.
//

#pragma once

#include "error.hpp"
#include "tcp_socket.hpp"

class ServerSocket {
public:
    ServerSocket() = default;
    ~ServerSocket() = default;

    Network::Error::Code Create();
    Network::Error::Code Close();
    Network::Error::Code Bind(unsigned short port);
    Network::Error::Code Listen(int backlog);
    Network::Error::Code Select();
    TCPSocket Accept();

    void InitSocketSet();
    void EnrollSocket(TCPSocket socket);
    void DropSocket(TCPSocket socket);

    bool IsServerSocketChanged();
    bool IsConnectedSocketChanged(TCPSocket socket);

private:
    TCPSocket socket_;

    fd_set read_fds_ {};
    fd_set temp_fds_ {};
    int fd_max_ = 0;
};



//
// Created by jwwoo on 2019-11-01.
//

#include <netinet/in.h>
#include "server_socket.hpp"

Error::Code ServerSocket::Create() {
    return socket_.Create();
}

Error::Code ServerSocket::Close() {
    return socket_.Close();
}

Error::Code ServerSocket::Bind(unsigned short port) {
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(socket_.GetSockfd(), (const struct sockaddr *) &address, sizeof(struct sockaddr_in)) != 0) {
        return Error::Code::BIND_SOCKET_FAIL;
    }

    return Error::Code::NONE;
}

Error::Code ServerSocket::Listen(int backlog) {
    if (listen(socket_.GetSockfd(), backlog) != 0) {
        return Error::Code::LISTEN_SOCKET_FAIL;
    }

    return Error::Code::NONE;
}

Error::Code ServerSocket::Select() {
    temp_fds_ = read_fds_;

    if (select(fd_max_ + 1, &temp_fds_, nullptr, nullptr, nullptr) == -1) {
        return Error::Code::SELECT_FAIL;
    }

    return Error::Code::NONE;
}

TCPSocket ServerSocket::Accept() {
    struct sockaddr_in client_address{};
    socklen_t client_addr_size = sizeof(client_address);

    int connected_sockfd = accept(socket_.GetSockfd(), (struct sockaddr *) &client_address, &client_addr_size);

    TCPSocket connected_socket;
    connected_socket.SetSockfd(connected_sockfd);

    return connected_socket;
}

void ServerSocket::InitSocketSet() {
    FD_ZERO(&read_fds_);
    FD_SET(socket_.GetSockfd(), &read_fds_);
    fd_max_ = socket_.GetSockfd();
}

void ServerSocket::EnrollSocket(TCPSocket socket) {
    FD_SET(socket.GetSockfd(), &read_fds_);

    if (fd_max_ < socket.GetSockfd()) {
        fd_max_ = socket.GetSockfd();
    }
}

void ServerSocket::DropSocket(TCPSocket socket) {
    FD_CLR(socket.GetSockfd(), &read_fds_);
    socket.Close();
}

bool ServerSocket::IsServerSocketChanged() {
    return FD_ISSET(socket_.GetSockfd(), &temp_fds_);
}

bool ServerSocket::IsConnectedSocketChanged(TCPSocket socket) {
    return FD_ISSET(socket.GetSockfd(), &temp_fds_);
}

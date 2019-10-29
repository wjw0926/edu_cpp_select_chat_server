//
// Created by jwW on 2019-10-18.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp_socket.hpp"

ErrorCode TCPSocket::CreateSocket() {
    if ((sockfd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        return ErrorCode::CREATE_SOCKET_FAIL;
    }

    return ErrorCode::NONE;
}

ErrorCode TCPSocket::BindSocket(unsigned short port) {
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(sockfd_, (const struct sockaddr *) &address, sizeof(struct sockaddr_in)) != 0) {
        CloseSocket();
        return ErrorCode::BIND_SOCKET_FAIL;
    }

    return ErrorCode::NONE;
}

ErrorCode TCPSocket::ListenSocket(int backlog) {
    if (listen(sockfd_, backlog) != 0) {
        return ErrorCode::LISTEN_SOCKET_FAIL;
    }

    return ErrorCode::NONE;
}

ErrorCode TCPSocket::AcceptSocket() {
    struct sockaddr_in client_address{};
    socklen_t client_addr_size = sizeof(client_address);

    if ((connected_sockfd_ = accept(sockfd_, (struct sockaddr *) &client_address, &client_addr_size)) == -1) {
        return ErrorCode::ACCEPT_SOCKET_FAIL;
    }

    return ErrorCode::NONE;
}

ErrorCode TCPSocket::ConnectSocket(unsigned short port) {
    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port);

    if (connect(sockfd_, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) != 0) {
        return ErrorCode::CONNECT_SOCKET_FAIL;
    }

    return ErrorCode::NONE;
}

ErrorCode TCPSocket::CloseSocket() {
    if (sockfd_ == -1) {
        return ErrorCode::SOCKET_NOT_EXIST;
    } else {
        if (close(sockfd_) != 0) {
            return ErrorCode::CLOSE_SOCKET_FAIL;
        }
    }

    return ErrorCode::NONE;
}

ErrorCode TCPSocket::SendData(int sockfd, const char *data, int size) {
    int bytes = send(sockfd, data, size, 0);

    if (bytes == -1) {
        return ErrorCode::SEND_FAIL;
    }

    return ErrorCode::NONE;
}

int TCPSocket::RecvData(int sockfd, char *data, int size) {
    return recv(sockfd, data, size, 0);
}

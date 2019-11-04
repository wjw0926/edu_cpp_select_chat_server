//
// Created by jwW on 2019-10-18.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "tcp_socket.hpp"

Error::Code TCPSocket::Create() {
    if ((sockfd_ = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        return Error::Code::CREATE_SOCKET_FAIL;
    }

    return Error::Code::NONE;
}

Error::Code TCPSocket::Close() {
    if (sockfd_ == -1) {
        return Error::Code::SOCKET_NOT_EXIST;
    } else {
        if (close(sockfd_) != 0) {
            return Error::Code::CLOSE_SOCKET_FAIL;
        }
    }

    return Error::Code::NONE;
}

Error::Code TCPSocket::Send(const char *data, int size) {
    int bytes = send(sockfd_, data, size, 0);

    if (bytes == -1) {
        return Error::Code::SEND_FAIL;
    }

    return Error::Code::NONE;
}

int TCPSocket::Receive(char *data, int size) {
    return recv(sockfd_, data, size, 0);
}

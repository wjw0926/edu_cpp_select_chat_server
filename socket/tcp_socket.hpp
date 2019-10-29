//
// Created by jwW on 2019-10-18.
//

#pragma once

#include "../common/error.hpp"
#include "../common/packet.hpp"

class TCPSocket {
public:
    TCPSocket() = default;
    ~TCPSocket() = default;

    int GetSockfd() { return sockfd_; };
    int GetConnectedSockfd() { return connected_sockfd_; };

    ErrorCode CreateSocket();
    ErrorCode BindSocket(unsigned short port);
    ErrorCode ListenSocket(int backlog);
    ErrorCode AcceptSocket();
    ErrorCode ConnectSocket(unsigned short port);
    ErrorCode CloseSocket();

    static ErrorCode SendData(int sockfd, const char *data, int size);
    static int RecvData(int sockfd, char *data, int size);

private:
    int sockfd_ = -1;
    int connected_sockfd_ = -1;
};

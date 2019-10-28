//
// Created by jwW on 2019-10-18.
//

#pragma once

#include "error.hpp"

class TCPSocket {
public:
    TCPSocket() = default;
    ~TCPSocket() = default;

    int GetSockfd() { return sockfd_; };
    int GetConnectedSockfd() { return connected_sockfd_; };

    Error::ErrorCode CreateSocket();
    Error::ErrorCode BindSocket(unsigned short port);
    Error::ErrorCode ListenSocket(int backlog);
    Error::ErrorCode AcceptSocket();
    Error::ErrorCode ConnectSocket(unsigned short port);
    Error::ErrorCode CloseSocket();

    static Error::ErrorCode SendData(int sockfd, const char *data, int size);
    static int RecvData(int sockfd, char *data, int size);

private:
    int sockfd_ = -1;
    int connected_sockfd_ = -1;
};

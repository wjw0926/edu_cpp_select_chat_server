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
    void SetSockfd(int sockfd) { sockfd_ = sockfd; };

    Error::Code Create();
    Error::Code Close();

    Error::Code Send(const char *data, int size);
    int Receive(char *data, int size);

private:
    int sockfd_ = -1;
};

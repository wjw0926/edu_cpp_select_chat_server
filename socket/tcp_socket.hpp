//
// Created by jwW on 2019-10-18.
//

#pragma once

class TCPSocket {
public:
    TCPSocket() = default;;
    ~TCPSocket() = default;

    int GetSockfd() { return sockfd_; };
    void SetSockfd(int sockfd) { sockfd_ = sockfd; };

private:
    int sockfd_ = 0;
};

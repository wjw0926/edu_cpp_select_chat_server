//
// Created by jwW on 2019-10-21.
//

#pragma once

#include <packet.hpp>
#include "../socket/tcp_socket.hpp"

class Server {
public:
    Server() = default;
    ~Server() = default;

    void Init();
    int Run(unsigned short port);

private:
    typedef void (*Func)(int sockfd, char *buffer, unsigned short total_size);
    Func FuncList[MAX_PACKET_ID] = { nullptr };

    static void FuncEcho(int sockfd, char *buffer, unsigned short total_size);
    static void FuncLogin(int sockfd, char *buffer, unsigned short total_size);
};

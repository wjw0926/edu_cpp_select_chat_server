//
// Created by jwW on 2019-10-21.
//

#pragma once

#include "../network/tcp_server.hpp"
#include "config.hpp"
#include "packet_manager.hpp"

class ChatServer : TCPServer {
public:
    ChatServer() = default;
    ~ChatServer() override = default;

    void Init(unsigned short port);
    void Start();
    void Stop();

    void OnAccept(int session_index) override;
    void OnClose(int session_index) override;
    void OnReceive(int session_index, char *data, unsigned short packet_size) override;
    
private:
    bool runnable_ = false;
    PacketManager packet_manager_;
};

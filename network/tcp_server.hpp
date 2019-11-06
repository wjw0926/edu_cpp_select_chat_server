//
// Created by jwwoo on 2019-10-31.
//

#pragma once

#include <deque>
#include <vector>
#include "error.hpp"
#include "config.hpp"
#include "session.hpp"
#include "server_socket.hpp"

class TCPServer {
public:
    TCPServer() = default;
    virtual ~TCPServer() = default;

    Network::Error::Code Init(unsigned short port, unsigned short packet_header_size);
    void Run();
    void End();

protected:
    void Send(int session_index, const char *data, int size);

private:
    void CreateSession();
    void CloseSession(int session_index);

    void Receive(int session_index);

    virtual void OnAccept(int session_index) {};
    virtual void OnClose(int session_index) {};
    virtual void OnReceive(int session_index, char *data, unsigned short packet_size) {};

    ServerSocket server_socket_;
    Network::Error error_;

    std::vector<Session> sessions_;
    std::deque<int> available_session_index_;

    unsigned short packet_header_size_ = 0;
};

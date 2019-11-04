//
// Created by jwwoo on 2019-10-31.
//

#pragma once

#include "tcp_socket.hpp"
#include "config.hpp"

class Session {
public:
    Session() = default;
    ~Session() = default;

    TCPSocket GetSocket() { return socket_; };
    void SetSocket(TCPSocket socket) { socket_ = socket; };

    int GetIndex() { return index_; };
    void SetIndex(int index) { index_ = index; };

    char *GetRecvBuffer() { return recv_buffer_; };
    char *GetSendBuffer() { return send_buffer_; };

    void ClearRecvBuffer();
    void ClearSendBuffer();

    unsigned int GetPrevRecvPos() { return prev_recv_pos_; };
    void SetPrevRecvPos(unsigned int prev_recv_pos) { prev_recv_pos_ = prev_recv_pos; };

    bool IsConnected() { return connected_; };
    void SetConnected() { connected_ = true; };
    void SetDisconnected() { connected_ = false; };

    Error::Code Send(const char *data, int size);
    int Receive(int size);

private:
    TCPSocket socket_;
    int index_ = -1;
    bool connected_ = false;

    char recv_buffer_[MAX_RECV_BUFFER_SIZE] {};
    char send_buffer_[MAX_SEND_BUFFER_SIZE] {};

    unsigned int prev_recv_pos_ = 0;
};



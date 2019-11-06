//
// Created by jwwoo on 2019-11-01.
//

#include <cstring>
#include "session.hpp"

Network::Error::Code Session::Send(const char *data, int size) {
    return socket_.Send(data, size);
}

int Session::Receive(int size) {
    return socket_.Receive(&recv_buffer_[prev_recv_pos_], size);
}

void Session::ClearRecvBuffer() {
    memset(recv_buffer_, 0, MAX_RECV_BUFFER_SIZE);
}

void Session::ClearSendBuffer() {
    memset(send_buffer_, 0, MAX_SEND_BUFFER_SIZE);
}

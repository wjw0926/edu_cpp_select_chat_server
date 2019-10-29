//
// Created by jwW on 2019-10-21.
//

#pragma once

#include <string>
#include <unordered_map>

enum class ErrorCode {
    NONE = 0,

    CREATE_SOCKET_FAIL = 1,
    BIND_SOCKET_FAIL = 2,
    LISTEN_SOCKET_FAIL = 3,
    CLOSE_SOCKET_FAIL = 4,
    ACCEPT_SOCKET_FAIL = 5,
    CONNECT_SOCKET_FAIL = 6,

    SOCKET_NOT_EXIST = 10,
    SEND_FAIL = 11,
    RECEIVE_FAIL = 21
};

const std::unordered_map<ErrorCode, std::string> error_map = {
        {ErrorCode::NONE,                "Not error"},

        {ErrorCode::CREATE_SOCKET_FAIL,  "Failed to create socket"},
        {ErrorCode::BIND_SOCKET_FAIL,    "Failed to bind socket"},
        {ErrorCode::LISTEN_SOCKET_FAIL,  "Failed to make socket listen"},
        {ErrorCode::CLOSE_SOCKET_FAIL,   "Failed to close socket"},
        {ErrorCode::ACCEPT_SOCKET_FAIL,  "Failed to accept"},
        {ErrorCode::CONNECT_SOCKET_FAIL, "Failed to connect"},

        {ErrorCode::SOCKET_NOT_EXIST,    "Socket is not exist"},
        {ErrorCode::SEND_FAIL,           "Failed to send data"},
        {ErrorCode::RECEIVE_FAIL,        "Failed to receive data"},
};

//
// Created by jwwoo on 2019-10-31.
//

#pragma once

#include <string>
#include <unordered_map>
#include <iostream>

namespace Network {
    class Error {
    public:
        enum Code {
            NONE = 0,

            CREATE_SOCKET_FAIL = 1,
            BIND_SOCKET_FAIL = 2,
            LISTEN_SOCKET_FAIL = 3,
            CLOSE_SOCKET_FAIL = 4,
            ACCEPT_SOCKET_FAIL = 5,
            CONNECT_SOCKET_FAIL = 6,

            SOCKET_NOT_EXIST = 10,
            SEND_FAIL = 11,
            RECEIVE_FAIL = 21,

            SELECT_FAIL = 30,
            SELECT_TIMEOUT = 31,
            MAX_CONNECTED_CLIENTS = 50
        };

        void PrintError(Code code) { std::cout << "ERROR: " << message_map_[code] << std::endl; };

    private:
        std::unordered_map<Code, std::string> message_map_ = {
                {Code::NONE,                  "Not error"},

                {Code::CREATE_SOCKET_FAIL,    "Failed to create socket"},
                {Code::BIND_SOCKET_FAIL,      "Failed to bind socket"},
                {Code::LISTEN_SOCKET_FAIL,    "Failed to make socket listen"},
                {Code::CLOSE_SOCKET_FAIL,     "Failed to close socket"},
                {Code::ACCEPT_SOCKET_FAIL,    "Failed to accept"},
                {Code::CONNECT_SOCKET_FAIL,   "Failed to connect"},

                {Code::SOCKET_NOT_EXIST,      "Socket is not exist"},
                {Code::SEND_FAIL,             "Failed to send data"},
                {Code::RECEIVE_FAIL,          "Failed to receive data"},

                {Code::SELECT_FAIL,           "Failed to select"},
                {Code::SELECT_TIMEOUT,        "Select timeout"},
                {Code::MAX_CONNECTED_CLIENTS, "Not enough capacity to accept more clients"}
        };
    };
}

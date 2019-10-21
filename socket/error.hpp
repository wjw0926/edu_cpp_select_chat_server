//
// Created by jwW on 2019-10-21.
//

#pragma once

enum class ErrorCode {
    NONE = 0;

    CREATE_SOCKET_FAIL = 1;
    BIND_SOCKET_FAIL = 2;
    LISTEN_SOCKET_FAIL = 3;
    ACCEPT_SOCKET_FAIL = 4;
    CONNECT_SOCKET_FAIL = 5;

    SEND_FAIL = 11;

    RECEIVE_FAIL = 21;
};

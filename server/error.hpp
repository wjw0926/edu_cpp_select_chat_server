//
// Created by jwwoo on 2019-11-13.
//

#pragma once

namespace Server {
    class Error {
    public:
        enum Code : unsigned short {
            NONE = 0,

            CONNECT_REDIS_FAIL = 101,

            USER_ID_DUPLICATED = 211,
            INVALID_SESSION_INDEX = 222,
            USER_NOT_CONNECTED = 223,
            USER_ALREADY_LOGIN = 224,
            USER_NOT_EXIST = 225,

            ROOM_ENTER_USER_NOT_LOGIN = 270,
            ROOM_ENTER_INVALID_ROOM_INDEX = 273,
            ROOM_ENTER_ROOM_IS_FULL = 276,

            ROOM_LEAVE_USER_NOT_IN_ROOM = 280,
            ROOM_LEAVE_INVALID_ROOM_INDEX = 288,
            ROOM_LEAVE_EMPTY_ROOM = 291,

            ROOM_CHAT_USER_NOT_IN_ROOM = 290,
            ROOM_CHAT_INVALID_ROOM_INDEX = 298
        };
    };
}

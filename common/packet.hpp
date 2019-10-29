//
// Created by jwW on 2019-10-18.
//

#pragma once

#include <string>

enum class PacketID : short {
    LOGIN_REQ = 10,
    LOGIN_RES = 11,

    ECHO_REQ = 100,
    ECHO_RES = 101
};

const int PACKET_HEADER_SIZE = 4;
const int MAX_CHAT_MSG_SIZE = 256;

#pragma pack(push, 1)

struct PacketHeader {
    short total_size;
    PacketID packet_id;
};

struct Packet : PacketHeader {
    char message[MAX_CHAT_MSG_SIZE];
};

#pragma pack(pop)

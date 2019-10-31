//
// Created by jwW on 2019-10-18.
//

#pragma once

#include <string>
#include <cstring>
#include <netinet/in.h>

enum class PacketID : unsigned short {
    LOGIN_REQ = 10,
    LOGIN_RES = 11,

    ECHO_REQ = 100,
    ECHO_RES = 101
};

const unsigned short MAX_PACKET_ID = 256;
const unsigned short MAX_PACKET_SIZE = 1024;
const unsigned short MAX_ECHO_LENGTH = 256;

#pragma pack(push, 1)

struct PacketHeader {
    unsigned short total_size;
    PacketID packet_id;
};

struct PacketEchoReq : PacketHeader {
    //unsigned short size;
    char message[MAX_ECHO_LENGTH];
};

struct PacketEchoRes : PacketHeader {
    //unsigned short size;
    char message[MAX_ECHO_LENGTH];
};

#pragma pack(pop)

const unsigned short PACKET_HEADER_SIZE = sizeof(PacketHeader);

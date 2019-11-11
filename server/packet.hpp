//
// Created by jwW on 2019-10-18.
//

#pragma once

enum class PacketID : unsigned short {
    SYS_ACCEPT = 1,
    SYS_CLOSE = 2,

    LOGIN_REQ = 21,
    LOGIN_RES = 22,

    ROOM_ENTER_REQ = 61,
    ROOM_ENTER_RES = 62,
    ROOM_ENTER_NTF = 63,

    ROOM_LEAVE_REQ = 66,
    ROOM_LEAVE_RES = 67,
    ROOM_LEAVE_NTF = 68,

    ROOM_CHAT_REQ = 76,
    ROOM_CHAT_RES = 77,
    ROOM_CHAT_NTF = 78,

    REDIS_LOGIN_REQ = 121,
    REDIS_LOGIN_RES = 122
};

struct PacketInfo {
    int session_index = -1;
    PacketID packet_id {};
    unsigned short body_size = 0;
    char* body = nullptr;
};

const int MAX_USER_ID_LENGTH = 16;
const int MAX_USER_PW_LENGTH = 16;
const int MAX_ROOM_TITLE_SIZE = 16;

#pragma pack(push, 1)

struct PacketHeader {
    unsigned short total_size = 0;
    PacketID packet_id {};
    unsigned char reserved = 0;
};

struct PacketLoginReq {
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
    char user_pw[MAX_USER_PW_LENGTH] = { 0, };
};

struct PacketLoginRes : PacketHeader {
    unsigned short result = 0;
};

struct PacketEnterRoomReq {
    bool created;
    short index;
    char title[MAX_ROOM_TITLE_SIZE];
};

struct PacketEnterRoomRes : PacketHeader {
    unsigned short result = 0;
};

struct PacketEnterRoomNtf {
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
};

struct PacketLeaveRoomReq {

};

struct PacketLeaveRoomRes : PacketHeader {
    unsigned short result = 0;
};

struct PacketLeaveRoomNft
{
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
};

#pragma pack(pop)

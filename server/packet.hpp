//
// Created by jwW on 2019-10-18.
//

#pragma once

enum class PacketID : unsigned short {
    SYS_ACCEPT = 1,
    SYS_CLOSE = 2,

    LOGIN_REQ = 21,
    LOGIN_RES = 22,

    REDIS_LOGIN_REQ = 121,
    REDIS_LOGIN_RES = 122,

    ROOM_ENTER_REQ = 61,
    ROOM_ENTER_RES = 62,
    ROOM_ENTER_NTF = 63,

    ROOM_LEAVE_REQ = 66,
    ROOM_LEAVE_RES = 67,
    ROOM_LEAVE_NTF = 68,

    ROOM_CHAT_REQ = 76,
    ROOM_CHAT_RES = 77,
    ROOM_CHAT_NTF = 78
};

struct PacketInfo {
    int session_index = -1;
    PacketID packet_id {};
    unsigned short body_size = 0;
    char* body = nullptr;
};

const int MAX_USER_ID_LENGTH = 16;
const int MAX_USER_PW_LENGTH = 16;
const int MAX_ROOM_CHAT_MSG_LENGTH = 256;

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
    unsigned short code = 0;
};

struct RedisPacketLoginReq {
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
    char user_pw[MAX_USER_PW_LENGTH] = { 0, };
};

struct RedisPacketLoginRes : PacketHeader {
    unsigned short code = 0;
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
};

struct PacketEnterRoomReq {
    int room_number;
};

struct PacketEnterRoomRes : PacketHeader {
    unsigned short code = 0;
};

struct PacketEnterRoomNtf : PacketHeader {
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
};

struct PacketLeaveRoomRes : PacketHeader {
    unsigned short code = 0;
};

struct PacketLeaveRoomNtf : PacketHeader {
    char user_id[MAX_USER_ID_LENGTH] = { 0, };
};

struct PacketChatRoomReq {
    unsigned short message_length = 0;
    char message[MAX_ROOM_CHAT_MSG_LENGTH] = { 0, };
};

struct PacketChatRoomRes : PacketHeader {
    unsigned short code = 0;
};

struct PacketChatRoomNtf : PacketHeader {
    char message[MAX_ROOM_CHAT_MSG_LENGTH] = { 0, };
};

#pragma pack(pop)

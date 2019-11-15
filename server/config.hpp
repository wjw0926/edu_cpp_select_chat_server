//
// Created by jwwoo on 2019-11-01.
//

#pragma once

#include "packet.hpp"

const unsigned short MAX_CONNECTED_USERS = 1000;
const unsigned short MAX_CREATED_ROOMS = 2000;
const unsigned short MAX_USERS_IN_ROOM = 3;

const unsigned short MAX_PACKET_ID = 256;
const unsigned short PACKET_HEADER_SIZE = sizeof(PacketHeader);

const std::string REDIS_HOST = "127.0.0.1";
const unsigned short REDIS_PORT = 6379;
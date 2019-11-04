//
// Created by jwwoo on 2019-11-01.
//

#pragma once

const unsigned short MAX_PACKET_SIZE = 1024;

const unsigned short MAX_RECV_BUFFER_SIZE = MAX_PACKET_SIZE * 2;
const unsigned short MAX_SEND_BUFFER_SIZE = MAX_PACKET_SIZE;

const int MAX_CONNECTED_SESSIONS = 1000;
const int BACKLOG = 5;

//
// Created by jwwoo on 2019-11-06.
//

#pragma once

#include <mutex>
#include <thread>
#include <deque>
#include "packet.hpp"
#include "error.hpp"

namespace RedisCpp { class CRedisConn; }

class RedisManager {
public:
    RedisManager() = default;
    ~RedisManager() = default;

    Server::Error::Code Connect(const std::string& host, unsigned short port);
    void Run();
    void End();

    void AddRedisReqPacketQueue(PacketInfo redis_packet_info);
    PacketInfo *GetRedisResPacket();

private:
    void ProcessRedisPacket();
    void AddRedisResPacketQueue(PacketInfo redis_packet_info);

    bool runnable_ = false;

    RedisCpp::CRedisConn *conn_{};

    std::thread thread_;

    std::mutex req_mutex_;
    std::mutex res_mutex_;
    std::deque<PacketInfo> req_packet_queue_;
    std::deque<PacketInfo> res_packet_queue_;
};



//
// Created by jwwoo on 2019-11-05.
//

#pragma once

#include <thread>
#include <mutex>
#include <deque>
#include <functional>

#include "config.hpp"
#include "error.hpp"
#include "user_manager.hpp"
#include "redis_manager.hpp"
#include "room_manager.hpp"

class PacketManager {
public:
    PacketManager() = default;
    ~PacketManager() = default;

    Server::Error::Code Init();
    void Run();
    void End();

    void AddPacketQueue(int session_index, PacketID packet_id, unsigned short body_size, const char* body);
    void AddPacketQueue(PacketInfo packet_info);

    std::function<void(int, const char *, int)> SendPacket;

    void (PacketManager::*FuncList_[MAX_PACKET_ID])(PacketInfo packet_info) = { nullptr };

private:
    void ProcessPacket();

    void FuncAccept(PacketInfo packet_info);
    void FuncClose(PacketInfo packet_info);
    void FuncLoginReq(PacketInfo packet_info);
    void FuncLoginRes(PacketInfo packet_info);
    void FuncEnterRoom(PacketInfo packet_info);
    void FuncLeaveRoom(PacketInfo packet_info);
    void FuncChatRoom(PacketInfo packet_info);

    bool runnable_ = false;

    UserManager user_manager_{};
    RedisManager redis_manager_{};
    RoomManager room_manager_{};

    std::thread thread_;
    std::mutex mutex_;
    std::deque<PacketInfo> packet_queue_;
};



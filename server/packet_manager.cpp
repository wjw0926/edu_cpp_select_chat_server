//
// Created by jwwoo on 2019-11-05.
//

#include <iostream>
#include <cstring>
#include "packet_manager.hpp"

bool PacketManager::Init() {
    user_manager_.Init();

    FuncList_[static_cast<int>(PacketID::SYS_ACCEPT)] = &PacketManager::FuncAccept;
    FuncList_[static_cast<int>(PacketID::SYS_CLOSE)] = &PacketManager::FuncClose;
    FuncList_[static_cast<int>(PacketID::LOGIN_REQ)] = &PacketManager::FuncLoginReq;
    FuncList_[static_cast<int>(PacketID::REDIS_LOGIN_RES)] = &PacketManager::FuncLoginRes;

    if(!redis_manager_.Connect(REDIS_HOST, REDIS_PORT)) {
        return false;
    }

    runnable_ = true;

    return true;
}

void PacketManager::Run() {
    thread_ = std::thread([this]() -> void { ProcessPacket(); });

    // Run redis manager thread to process redis task
    redis_manager_.Run();
}

void PacketManager::End() {
    runnable_ = false;

    redis_manager_.End();

    if (thread_.joinable())
    {
        thread_.join();
    }
}

void PacketManager::AddPacketQueue(int session_index, PacketID packet_id, unsigned short body_size, const char* body) {
    PacketInfo packet_info = { session_index, packet_id, body_size, const_cast<char *>(body) };

    AddPacketQueue(packet_info);
}

void PacketManager::AddPacketQueue(PacketInfo packet_info) {
    std::unique_lock<std::mutex> lock(mutex_);
    packet_queue_.emplace_back(packet_info);
    lock.unlock();
}

void PacketManager::ProcessPacket() {
    while (runnable_) {
        auto redis_packet_info = redis_manager_.GetRedisResPacket();

        if (redis_packet_info != nullptr) {
            AddPacketQueue(*redis_packet_info);
        }

        std::unique_lock<std::mutex> lock(mutex_);
        if (!packet_queue_.empty()) {
            PacketInfo &packet_info = packet_queue_.front();
            packet_queue_.pop_front();
            lock.unlock();

            if (static_cast<int>(packet_info.packet_id) < 0 || static_cast<int>(packet_info.packet_id) > 256) {
                continue;
            }

            if (FuncList_[static_cast<int>(packet_info.packet_id)] == nullptr) {
                continue;
            }

            (this->*FuncList_[static_cast<int>(packet_info.packet_id)])(packet_info);
        }
    }
}

void PacketManager::FuncAccept(PacketInfo packet_info) {
    user_manager_.AddUser(packet_info.session_index);
}

void PacketManager::FuncClose(PacketInfo packet_info) {
    user_manager_.RemoveUser(packet_info.session_index);
}

void PacketManager::FuncLoginReq(PacketInfo packet_info) {
    auto req = reinterpret_cast<PacketLoginReq *>(packet_info.body);

    std::cout << "SERVER: Received login request: " << req->user_id << ", " << req->user_pw << std::endl;

    PacketLoginRes res{};
    res.packet_id = PacketID::LOGIN_RES;
    res.total_size = sizeof(PacketLoginRes);

    auto user = user_manager_.GetUser(packet_info.session_index);

    if (user == nullptr) {
        res.result = 1;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    }

    if (user->GetState() == User::State::NONE) {
        res.result = 1;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    } else if (user->GetState() == User::State::LOGIN) {
        res.result = 1;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    }

    PacketLoginReq redis_req;
    memcpy(redis_req.user_id, req->user_id, MAX_USER_ID_LENGTH);
    memcpy(redis_req.user_pw, req->user_pw, MAX_USER_PW_LENGTH);

    PacketInfo redis_packet_info;
    redis_packet_info.session_index = user->GetIndex();
    redis_packet_info.packet_id = PacketID::REDIS_LOGIN_REQ;
    redis_packet_info.body_size = sizeof(PacketLoginReq);
    redis_packet_info.body = new char[sizeof(PacketLoginReq)];
    memcpy(redis_packet_info.body, reinterpret_cast<char *>(&redis_req), sizeof(PacketLoginReq));
    redis_manager_.AddRedisReqPacketQueue(redis_packet_info);
}

void PacketManager::FuncLoginRes(PacketInfo packet_info) {
    auto user = user_manager_.GetUser(packet_info.session_index);

    PacketLoginRes res{};
    res.packet_id = PacketID::LOGIN_RES;
    res.total_size = sizeof(PacketLoginRes);
    res.result = static_cast<unsigned short>(*packet_info.body);

    std::cout << "SERVER: Send response..." << std::endl;

    if (res.result == 0) {
        user->SetState(User::State::LOGIN);
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
    } else {
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
    }
}

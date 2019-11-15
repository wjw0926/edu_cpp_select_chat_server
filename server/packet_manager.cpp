//
// Created by jwwoo on 2019-11-05.
//

#include <iostream>
#include <cstring>
#include "packet_manager.hpp"

Server::Error::Code PacketManager::Init() {
    user_manager_.Init();
    room_manager_.Init();

    auto send_packet_func = [&](int session_index, const char *data, int size) { SendPacket(session_index, data, size); };
    room_manager_.SendPacket = send_packet_func;

    FuncList_[static_cast<int>(PacketID::SYS_ACCEPT)] = &PacketManager::FuncAccept;
    FuncList_[static_cast<int>(PacketID::SYS_CLOSE)] = &PacketManager::FuncClose;
    FuncList_[static_cast<int>(PacketID::LOGIN_REQ)] = &PacketManager::FuncLoginReq;
    FuncList_[static_cast<int>(PacketID::REDIS_LOGIN_RES)] = &PacketManager::FuncLoginRes;
    FuncList_[static_cast<int>(PacketID::ROOM_ENTER_REQ)] = &PacketManager::FuncEnterRoom;
    FuncList_[static_cast<int>(PacketID::ROOM_LEAVE_REQ)] = &PacketManager::FuncLeaveRoom;
    FuncList_[static_cast<int>(PacketID::ROOM_CHAT_REQ)] = &PacketManager::FuncChatRoom;

    if(redis_manager_.Connect(REDIS_HOST, REDIS_PORT) == Server::Error::CONNECT_REDIS_FAIL) {
        return Server::Error::CONNECT_REDIS_FAIL;
    }

    runnable_ = true;

    return Server::Error::NONE;
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

            if (static_cast<int>(packet_info.packet_id) > MAX_PACKET_ID) {
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
    auto user = user_manager_.GetUser(packet_info.session_index);

    switch (user->GetState()) {
        case User::State::NONE:
            std::cout << "SERVER: fatal error: call OnClose from unconnected session" << std::endl;
            user->Clear();
            break;
        case User::State::CONNECT:
            user_manager_.RemoveUser(packet_info.session_index);
            break;
        case User::State::LOGIN:
            user_manager_.RemoveUser(packet_info.session_index);
            break;
        case User::State::ROOM:
            room_manager_.LeaveRoom(*user);
            user_manager_.RemoveUser(packet_info.session_index);
            break;
    }
}

void PacketManager::FuncLoginReq(PacketInfo packet_info) {
    auto req = reinterpret_cast<PacketLoginReq *>(packet_info.body);
    std::cout << "SERVER: Received login request: " << req->user_id << ", " << req->user_pw << std::endl;

    PacketLoginRes res{};
    res.packet_id = PacketID::LOGIN_RES;
    res.total_size = sizeof(PacketLoginRes);

    auto user = user_manager_.GetUser(packet_info.session_index);

    if (user == nullptr) {
        res.code = Server::Error::Code::INVALID_SESSION_INDEX;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    }

    if (user->GetState() == User::State::NONE) {
        res.code = Server::Error::Code::USER_NOT_CONNECTED;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    }

    if (user->GetState() == User::State::LOGIN) {
        res.code = Server::Error::Code::USER_ALREADY_LOGIN;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    }

    if (user_manager_.IsDuplicatedUserID(req->user_id)) {
        res.code = Server::Error::Code::USER_ID_DUPLICATED;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
        return;
    }

    RedisPacketLoginReq redis_req;
    memcpy(redis_req.user_id, req->user_id, MAX_USER_ID_LENGTH);
    memcpy(redis_req.user_pw, req->user_pw, MAX_USER_PW_LENGTH);

    PacketInfo redis_packet_info;
    redis_packet_info.session_index = user->GetIndex();
    redis_packet_info.packet_id = PacketID::REDIS_LOGIN_REQ;
    redis_packet_info.body_size = sizeof(RedisPacketLoginReq);
    redis_packet_info.body = new char[sizeof(RedisPacketLoginReq)];
    memcpy(redis_packet_info.body, reinterpret_cast<char *>(&redis_req), sizeof(RedisPacketLoginReq));

    redis_manager_.AddRedisReqPacketQueue(redis_packet_info);
}

void PacketManager::FuncLoginRes(PacketInfo packet_info) {
    PacketLoginRes res{};
    res.packet_id = PacketID::LOGIN_RES;
    res.total_size = sizeof(PacketLoginRes);

    auto user = user_manager_.GetUser(packet_info.session_index);
    auto redis_res = reinterpret_cast<RedisPacketLoginRes *>(packet_info.body);
    res.code = redis_res->code;

    if (res.code == Server::Error::Code::NONE) {
        user->SetState(User::State::LOGIN);
        user->SetUserID(redis_res->user_id);
    }

    std::cout << "SERVER: Sending login response..." << std::endl;
    SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLoginRes));
}

void PacketManager::FuncEnterRoom(PacketInfo packet_info) {
    // Prepare response packet
    PacketEnterRoomRes res{};
    res.packet_id = PacketID::ROOM_ENTER_RES;
    res.total_size = sizeof(PacketEnterRoomRes);

    // Check user is log-inned
    auto user = user_manager_.GetUser(packet_info.session_index);

    if (user->GetState() != User::State::LOGIN) {
        res.code = Server::Error::Code::ROOM_ENTER_USER_NOT_LOGIN;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketEnterRoomRes));
        return;
    }

    // Check room number is valid
    auto req = reinterpret_cast<PacketEnterRoomReq *>(packet_info.body);

    if (req->room_number < 0 || req->room_number > MAX_CREATED_ROOMS) {
        res.code = Server::Error::Code::ROOM_ENTER_INVALID_ROOM_INDEX;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketEnterRoomRes));
        return;
    }

    // Check room is not full to enter
    auto room = room_manager_.GetRoom(req->room_number);

    if (room->IsFull()) {
        res.code = Server::Error::Code::ROOM_ENTER_ROOM_IS_FULL;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketEnterRoomRes));
        return;
    }

    // Normal execution: enter room
    room_manager_.EnterRoom(*user, room->GetIndex());
    res.code = Server::Error::Code::NONE;
    SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketEnterRoomRes));

    // Notify users in the room about new user
    PacketEnterRoomNtf ntf;
    ntf.packet_id = PacketID::ROOM_ENTER_NTF;
    ntf.total_size = sizeof(PacketEnterRoomNtf);
    memcpy(ntf.user_id, user->GetUserID(), MAX_USER_ID_LENGTH);

    room->NotifyUsers(reinterpret_cast<const char *>(&ntf), sizeof(PacketEnterRoomNtf));
}

void PacketManager::FuncLeaveRoom(PacketInfo packet_info) {
    // Prepare response packet
    PacketLeaveRoomRes res{};
    res.packet_id = PacketID::ROOM_LEAVE_RES;
    res.total_size = sizeof(PacketLeaveRoomRes);

    // Check user is in room
    auto user = user_manager_.GetUser(packet_info.session_index);

    if (user->GetState() != User::State::ROOM) {
        res.code = Server::Error::Code::ROOM_LEAVE_USER_NOT_IN_ROOM;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLeaveRoomRes));
        return;
    }

    // Check room number is valid
    if (user->GetRoomIndex() < 0 || user->GetRoomIndex() > MAX_CREATED_ROOMS) {
        res.code = Server::Error::Code::ROOM_LEAVE_INVALID_ROOM_INDEX;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLeaveRoomRes));
        return;
    }

    // Check room is not empty to leave
    auto room = room_manager_.GetRoom(user->GetRoomIndex());

    if (room->IsEmpty()) {
        res.code = Server::Error::Code::ROOM_LEAVE_EMPTY_ROOM;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLeaveRoomRes));
        return;
    }

    // Normal execution: leave room
    room_manager_.LeaveRoom(*user);
    res.code = Server::Error::Code::NONE;
    SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketLeaveRoomRes));

    // Notify users in the room about leaving user
    PacketLeaveRoomNtf ntf;
    ntf.packet_id = PacketID::ROOM_LEAVE_NTF;
    ntf.total_size = sizeof(PacketLeaveRoomNtf);
    memcpy(ntf.user_id, user->GetUserID(), MAX_USER_ID_LENGTH);

    room->NotifyUsers(reinterpret_cast<const char *>(&ntf), sizeof(PacketLeaveRoomNtf));
}

void PacketManager::FuncChatRoom(PacketInfo packet_info) {
    // Prepare response packet
    PacketChatRoomRes res{};
    res.packet_id = PacketID::ROOM_CHAT_RES;
    res.total_size = sizeof(PacketChatRoomRes);

    // Check user is in room
    auto user = user_manager_.GetUser(packet_info.session_index);

    if (user->GetState() != User::State::ROOM) {
        res.code = Server::Error::Code::ROOM_CHAT_USER_NOT_IN_ROOM;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketChatRoomRes));
        return;
    }

    // Check room number is valid
    if (user->GetRoomIndex() < 0 || user->GetRoomIndex() > MAX_CREATED_ROOMS) {
        res.code = Server::Error::Code::ROOM_CHAT_INVALID_ROOM_INDEX;
        SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketChatRoomRes));
        return;
    }

    // Normal execution: chat
    auto req = reinterpret_cast<PacketChatRoomReq *>(packet_info.body);
    auto room = room_manager_.GetRoom(user->GetRoomIndex());

    PacketChatRoomNtf ntf;
    ntf.packet_id = PacketID::ROOM_CHAT_NTF;
    ntf.total_size = sizeof(PacketChatRoomNtf);
    memcpy(ntf.message, req->message, req->message_length);

    room->NotifyUsers(reinterpret_cast<const char *>(&ntf), sizeof(PacketChatRoomNtf));

    // Send response to chatted user
    res.code = Server::Error::Code::NONE;
    SendPacket(packet_info.session_index, reinterpret_cast<const char *>(&res), sizeof(PacketChatRoomRes));
}

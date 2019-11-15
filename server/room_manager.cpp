//
// Created by jwwoo on 2019-11-11.
//

#include "room_manager.hpp"
#include "config.hpp"

void RoomManager::Init() {
    auto send_packet_func = [&](int session_index, const char *data, int size) { SendPacket(session_index, data, size); };

    for (int i = 0; i < MAX_CREATED_ROOMS; ++i) {
        rooms_.emplace_back(Room(i));
        rooms_[i].SendPacket = send_packet_func;
    }
}

void RoomManager::EnterRoom(User &user, int room_index) {
    auto room = GetRoom(room_index);

    room->AddUser(user);
    user.SetState(User::State::ROOM);
    user.SetRoomIndex(room_index);
}

void RoomManager::LeaveRoom(User &user) {
    auto room = GetRoom(user.GetRoomIndex());

    room->RemoveUser(user);
    user.SetState(User::State::LOGIN);
    user.ClearRoomIndex();
}

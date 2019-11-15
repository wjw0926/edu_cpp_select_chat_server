//
// Created by jwwoo on 2019-11-11.
//

#pragma once

#include <vector>
#include <functional>
#include "room.hpp"
#include "user.hpp"

class RoomManager {
public:
    RoomManager() = default;
    ~RoomManager() = default;

    std::function<void(int, const char *, int)> SendPacket;

    void Init();

    void EnterRoom(User &user, int room_index);
    void LeaveRoom(User &user);
    Room *GetRoom(int index) { return &rooms_.at(index); };

private:
    std::vector<Room> rooms_;
};

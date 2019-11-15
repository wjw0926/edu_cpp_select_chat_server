//
// Created by jwwoo on 2019-11-11.
//

#pragma once

#include <list>
#include <functional>
#include "config.hpp"
#include "user.hpp"

class Room {
public:
    explicit Room(int index) { index_ = index; };
    ~Room() = default;

    std::function<void(int, const char *, int)> SendPacket;

    int GetIndex() { return index_; };

    bool IsFull() { return users_.size() >= MAX_USERS_IN_ROOM; };
    bool IsEmpty() { return users_.empty(); };

    void AddUser(User &user) { users_.push_back(&user); };
    void RemoveUser(User &user) { users_.remove(&user); };

    void NotifyUsers(const char *packet, unsigned short size) {
        for (auto user : users_) {
            SendPacket(user->GetIndex(), packet, size);
        }
    };

private:
    int index_ = -1;
    std::list<User*> users_;
};

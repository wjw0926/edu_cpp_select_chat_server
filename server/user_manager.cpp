//
// Created by jwwoo on 2019-11-01.
//

#include <iostream>
#include "user_manager.hpp"
#include "config.hpp"

void UserManager::Init() {
    for (int i = 0; i < MAX_CONNECTED_USERS; ++i) {
        users_.emplace_back(User());
    }
}

void UserManager::AddUser(const int session_index) {
    users_[session_index].SetIndex(session_index);
    users_[session_index].SetState(User::State::CONNECT);

    index_user_map_[session_index] = &users_[session_index];
}

void UserManager::RemoveUser(const int session_index) {
    auto user = GetUser(session_index);
    user->SetState(User::State::NONE);

    index_user_map_.erase(session_index);
}

User* UserManager::GetUser(int index) {
    if (index_user_map_.find(index) == index_user_map_.end()) {
        return nullptr;
    }

    return index_user_map_[index];
}

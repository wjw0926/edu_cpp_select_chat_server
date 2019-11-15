//
// Created by jwwoo on 2019-11-01.
//

#pragma once

#include <vector>
#include <unordered_map>
#include "user.hpp"

class UserManager {
public:
    UserManager() = default;
    ~UserManager() = default;

    void Init();

    void AddUser(int session_index);
    void RemoveUser(int session_index);
    User* GetUser(int index);
    bool IsDuplicatedUserID(char *user_id);

private:
    std::vector<User> users_;
    std::unordered_map<int, User*> index_user_map_;
};

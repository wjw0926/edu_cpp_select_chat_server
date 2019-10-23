//
// Created by jwW on 2019-10-21.
//

#pragma once

class Client {
public:
    Client() = default;
    ~Client() = default;

    static int Run(unsigned short port);
};

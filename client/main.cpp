//
// Created by jwW on 2019-10-23.
//

#include <iostream>
#include "client.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: select_chat_client <port>" << std::endl;
        return 0;
    } else {
        return Client::Run(static_cast<unsigned short>(std::stoul(argv[1])));
    }
}
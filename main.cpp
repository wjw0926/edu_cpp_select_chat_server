#include <iostream>
#include "server/server.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: select_chat_server <port>" << std::endl;
        return 0;
    } else {
        return Server::Run(static_cast<unsigned short>(std::stoul(argv[1])));
    }
}
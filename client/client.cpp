//
// Created by jwW on 2019-10-21.
//

#include <iostream>
#include <cstring>
#include <memory>
#include "client.hpp"
#include "../socket/tcp_socket.hpp"

int Client::Run(unsigned short port) {
    auto socket = std::make_unique<TCPSocket>();

    if (socket->CreateSocket() == ErrorCode::CREATE_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::CREATE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    if (socket->ConnectSocket(port) == ErrorCode::CONNECT_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::CONNECT_SOCKET_FAIL) << std::endl;

        if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
            std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
            return -1;
        }

        return -1;
    }

    std::cout << "Connected! Type \"done\" to exit." << std::endl;

    char buffer[256];

    while (strcmp(buffer, "done") != 0) {
        std::cout << "Message: ";

        fgets(buffer, 256, stdin);
        buffer[strlen(buffer) - 1] = '\0';

        std::cout << "Sending..." << std::endl;

        if (socket->SendData(socket->GetSockfd(), reinterpret_cast<const char *>(&buffer), sizeof(buffer)) == ErrorCode::SEND_FAIL) {
            std::cout << "Error: " << error_map.at(ErrorCode::SEND_FAIL) << std::endl;
            break;
        }

        int recv = socket->RecvData(socket->GetSockfd(), reinterpret_cast<char *>(&buffer), sizeof(buffer));

        if (recv == -1) {
            std::cout << "Error: " << error_map.at(ErrorCode::RECEIVE_FAIL) << std::endl;
            break;
        } else if (recv == 0) {
            std::cout << "Disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;
    }

    std::cout << "Done!" << std::endl;

    if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    return 0;
}

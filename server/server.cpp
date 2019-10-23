//
// Created by jwW on 2019-10-21.
//

#include <iostream>
#include <cstring>
#include <memory>
#include "server.hpp"
#include "../socket/tcp_socket.hpp"

int Server::Run(unsigned short port) {
    auto socket = std::make_unique<TCPSocket>();
    Error error = Error();

    if (socket->CreateSocket() == Error::ErrorCode::CREATE_SOCKET_FAIL) {
        std::cout << "Error: " << error.error_map.at(Error::ErrorCode::CREATE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    if (socket->BindSocket(port) == Error::ErrorCode::BIND_SOCKET_FAIL) {
        std::cout << "Error: " << error.error_map.at(Error::ErrorCode::BIND_SOCKET_FAIL) << std::endl;
        return -1;
    }

    if (socket->ListenSocket(5) == Error::ErrorCode::LISTEN_SOCKET_FAIL) {
        std::cout << "Error: " << error.error_map.at(Error::ErrorCode::LISTEN_SOCKET_FAIL) << std::endl;
        return -1;
    }

    std::cout << "Running server on port " << port << std::endl;

    if (socket->AcceptSocket() == Error::ErrorCode::ACCEPT_SOCKET_FAIL) {
        std::cout << "Error: " << error.error_map.at(Error::ErrorCode::ACCEPT_SOCKET_FAIL) << std::endl;
        return -1;
    }

    std::cout << "Accept client on port " << port << std::endl;

    char buffer[256];

    while (strcmp(buffer, "done") != 0) {
        memset(buffer, 0, sizeof(buffer));
        int recv = socket->RecvData(socket->GetClientSockfd(), reinterpret_cast<char *>(&buffer), sizeof(buffer));

        if (recv == -1) {
            std::cout << "Error: " << error.error_map.at(Error::ErrorCode::RECEIVE_FAIL) << std::endl;
            break;
        } else if (recv == 0) {
            std::cout << "Disconnected" << std::endl;
            break;
        }

        std::cout << "Received: " << buffer << std::endl;

        std::cout << "Echoing..." << std::endl;

        if (socket->SendData(socket->GetClientSockfd(), reinterpret_cast<char *>(&buffer), sizeof(buffer)) == Error::ErrorCode::SEND_FAIL) {
            std::cout << "Error: " << error.error_map.at(Error::ErrorCode::SEND_FAIL) << std::endl;
        }
    }

    if (socket->CloseSocket() == Error::ErrorCode::CLOSE_SOCKET_FAIL) {
        std::cout << "Error: " << error.error_map.at(Error::ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    return 0;
}

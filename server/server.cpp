//
// Created by jwW on 2019-10-21.
//

#include <iostream>
#include <memory>
#include "server.hpp"
#include "../socket/tcp_socket.hpp"

void Server::Init() {
    FuncList[static_cast<int>(PacketID::ECHO_REQ)] = FuncEcho;
    FuncList[static_cast<int>(PacketID::LOGIN_REQ)] = FuncLogin;
}

int Server::Run(unsigned short port) {
    auto socket = std::make_unique<TCPSocket>();

    if (socket->CreateSocket() == ErrorCode::CREATE_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::CREATE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    if (socket->BindSocket(port) == ErrorCode::BIND_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::BIND_SOCKET_FAIL) << std::endl;

        if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
            std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
            return -1;
        }

        return -1;
    }

    if (socket->ListenSocket(5) == ErrorCode::LISTEN_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::LISTEN_SOCKET_FAIL) << std::endl;

        if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
            std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
            return -1;
        }

        return -1;
    }

    std::cout << "Running server on port " << port << std::endl;

    if (socket->AcceptSocket() == ErrorCode::ACCEPT_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::ACCEPT_SOCKET_FAIL) << std::endl;

        if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
            std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
            return -1;
        }

        return -1;
    }

    std::cout << "Accept client on port " << port << std::endl;

    unsigned short recv_pos = 0;

    while (true) {
        char recv_buffer[MAX_PACKET_SIZE * 2];

        int recv = socket->RecvData(socket->GetConnectedSockfd(), &recv_buffer[recv_pos]);

        if (recv == -1) {
            std::cout << "Error: " << error_map.at(ErrorCode::RECEIVE_FAIL) << std::endl;
            break;
        } else if (recv == 0) {
            std::cout << "Disconnected" << std::endl;
            break;
        }

        // Process received data
        unsigned short read_pos = 0;
        auto remain_data = recv_pos + recv;

        while (remain_data >= PACKET_HEADER_SIZE) {
            auto header = reinterpret_cast<PacketHeader *>(&recv_buffer[read_pos]);

            if (header->total_size > remain_data) {
                break;
            }

            // Execute packet function
            (*FuncList[static_cast<int>(header->packet_id)])(socket->GetConnectedSockfd(), &recv_buffer[read_pos], header->total_size);

            read_pos += header->total_size;
            remain_data -= header->total_size;
        }

        // Move remain data to the first position of receive buffer
        if (remain_data > 0) {
            memcpy(recv_buffer, &recv_buffer[read_pos], remain_data);
        }
        recv_pos = remain_data;
    }

    if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    return 0;
}

void Server::FuncEcho(int sockfd, char *buffer, unsigned short total_size) {
    PacketEchoReq req{};
    PacketEchoRes res{};

    memcpy(&req, buffer, total_size);

    std::cout << "Received PacketID: " << static_cast<unsigned short>(req.packet_id) << ", "
              << req.message << " (" << req.total_size << " bytes)" << std::endl;

    res.packet_id = PacketID::ECHO_RES;
    res.total_size = req.total_size;
    strncpy(res.message, req.message, req.total_size);

    std::cout << "Echoing..." << std::endl;

    auto send_buffer = reinterpret_cast<const char *>(&res);

    if (TCPSocket::SendData(sockfd, send_buffer, sizeof(PacketEchoRes)) == ErrorCode::SEND_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::SEND_FAIL) << std::endl;
    }
}

void Server::FuncLogin(int sockfd, char *buffer, unsigned short total_size) {

}

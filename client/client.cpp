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

    struct PacketEchoReq echo_req{};
    unsigned short recv_pos = 0;

    while (strcmp(echo_req.message, "done") != 0) {
        std::cout << "Message: ";

        fgets(echo_req.message, MAX_ECHO_LENGTH, stdin);
        echo_req.message[strlen(echo_req.message) - 1] = '\0';
        echo_req.total_size = MAX_ECHO_LENGTH + PACKET_HEADER_SIZE;
        echo_req.packet_id = PacketID::ECHO_REQ;

        auto buffer = reinterpret_cast<const char *>(&echo_req);

        // Sending echo packet
        std::cout << "Sending..." << std::endl;
        if (socket->SendData(socket->GetSockfd(), buffer, sizeof(PacketEchoReq)) == ErrorCode::SEND_FAIL) {
            std::cout << "Error: " << error_map.at(ErrorCode::SEND_FAIL) << std::endl;
            break;
        }

        // Receiving echo packet
        char recv_buffer[MAX_PACKET_SIZE * 2];

        int recv = socket->RecvData(socket->GetSockfd(), &recv_buffer[recv_pos]);

        if (recv == -1) {
            std::cout << "Error: " << error_map.at(ErrorCode::RECEIVE_FAIL) << std::endl;
            break;
        } else if (recv == 0) {
            std::cout << "Disconnected" << std::endl;
            break;
        }

        // Process received data
        auto read_pos = 0;
        unsigned short remain_data = recv_pos + recv;

        while (remain_data >= PACKET_HEADER_SIZE) {
            auto header = reinterpret_cast<PacketHeader *>(&recv_buffer[read_pos]);

            if (header->total_size > remain_data) {
                break;
            }

            // Echo message from packet
            if (header->packet_id == PacketID::ECHO_RES) {
                auto echo_res = reinterpret_cast<PacketEchoRes *>(&recv_buffer[read_pos]);

                std::cout << "Received PacketID: " << static_cast<unsigned short>(echo_res->packet_id) << ", "
                          << echo_res->message << " (" << echo_res->total_size << " bytes)" << std::endl;
            }

            read_pos += header->total_size;
            remain_data -= header->total_size;
        }

        // Move remain data to the first position of receive buffer
        if (remain_data > 0) {
            memcpy(recv_buffer, &recv_buffer[read_pos], remain_data);
        }
        recv_pos = remain_data;
    }

    std::cout << "Done!" << std::endl;

    if (socket->CloseSocket() == ErrorCode::CLOSE_SOCKET_FAIL) {
        std::cout << "Error: " << error_map.at(ErrorCode::CLOSE_SOCKET_FAIL) << std::endl;
        return -1;
    }

    return 0;
}

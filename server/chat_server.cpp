//
// Created by jwW on 2019-10-21.
//

#include "chat_server.hpp"
#include "error.hpp"
#include "config.hpp"

void ChatServer::Init(unsigned short port) {
    Network::Error::Code network_code = TCPServer::Init(port, PACKET_HEADER_SIZE);

    if (network_code != Network::Error::Code::NONE) {
        network_error_.PrintError(network_code);
        return;
    }

    if(packet_manager_.Init() != Server::Error::Code::NONE) {
        return;
    }

    auto send_packet_func = [&](int session_index, const char *data, int size) { Send(session_index, data, size); };
    packet_manager_.SendPacket = send_packet_func;

    runnable_ = true;
}

void ChatServer::Start() {
    // Run packet manager thread to process packet
    packet_manager_.Run();

    while (runnable_) {
        TCPServer::Run();
    }
}

void ChatServer::Stop() {
    runnable_ = false;
    packet_manager_.End();
    TCPServer::End();
}

void ChatServer::OnAccept(int session_index) {
    packet_manager_.AddPacketQueue(session_index, PacketID::SYS_ACCEPT, 0, nullptr);
}

void ChatServer::OnClose(int session_index) {
    packet_manager_.AddPacketQueue(session_index, PacketID::SYS_CLOSE, 0, nullptr);
}

void ChatServer::OnReceive(int session_index, char *data, unsigned short packet_size) {
    auto header = reinterpret_cast<PacketHeader *>(data);

    if (header->total_size != packet_size) {
        return;
    }

    packet_manager_.AddPacketQueue(session_index, header->packet_id, packet_size - PACKET_HEADER_SIZE, &data[PACKET_HEADER_SIZE]);
}

//
// Created by jwwoo on 2019-10-31.
//

#include <iostream>
#include <cstring>
#include "tcp_server.hpp"

Error::Code TCPServer::Init(unsigned short port, unsigned short packet_header_size) {
    for (int i = 0; i < MAX_CONNECTED_SESSIONS; ++i) {
        sessions_.emplace_back(Session());
        available_session_index_.emplace_back(i);
    }

    packet_header_size_ = packet_header_size;

    if (server_socket_.Create() == Error::Code::CREATE_SOCKET_FAIL) {
        return Error::Code::CREATE_SOCKET_FAIL;
    }

    if (server_socket_.Bind(port) == Error::Code::BIND_SOCKET_FAIL) {
        if (server_socket_.Close() == Error::Code::CLOSE_SOCKET_FAIL) {
            return Error::Code::CLOSE_SOCKET_FAIL;
        }

        return Error::Code::BIND_SOCKET_FAIL;
    }

    if (server_socket_.Listen(BACKLOG) == Error::Code::LISTEN_SOCKET_FAIL) {
        if (server_socket_.Close() == Error::Code::CLOSE_SOCKET_FAIL) {
            return Error::Code::CLOSE_SOCKET_FAIL;
        }

        return Error::Code::LISTEN_SOCKET_FAIL;
    }

    server_socket_.InitSocketSet();

    std::cout << "NETWORK: Running server on port " << port << std::endl;

    return Error::Code::NONE;
}

void TCPServer::Run() {
    if (server_socket_.Select() == Error::Code::SELECT_FAIL) {
        std::cout << "ERROR: " << error_.GetMessageMap().at(Error::Code::SELECT_FAIL) << std::endl;
        return;
    }

    if (server_socket_.IsServerSocketChanged()) {
        CreateSession();
    } else {
        for (auto & session : sessions_) {
            if (session.IsConnected()) {
                if (server_socket_.IsConnectedSocketChanged(session.GetSocket())) {
                    Receive(session.GetIndex());
                }
            }
        }
    }
}

void TCPServer::CreateSession() {
    if (available_session_index_.empty()) {
        std::cout << "ERROR: " << error_.GetMessageMap().at(Error::Code::MAX_CONNECTED_CLIENTS) << std::endl;
        return;
    }

    TCPSocket connected_socket = server_socket_.Accept();

    if (connected_socket.GetSockfd() == -1) {
        std::cout << "ERROR: " << error_.GetMessageMap().at(Error::Code::ACCEPT_SOCKET_FAIL) << std::endl;
        return;
    }

    int session_index = available_session_index_.front();
    available_session_index_.pop_front();

    sessions_[session_index].SetSocket(connected_socket);
    sessions_[session_index].SetIndex(session_index);
    sessions_[session_index].SetConnected();

    server_socket_.EnrollSocket(sessions_[session_index].GetSocket());

    std::cout << "NETWORK: Accept client session " << session_index << std::endl;

    OnAccept(session_index);
}

void TCPServer::CloseSession(const int session_index) {
    OnClose(session_index);

    server_socket_.DropSocket(sessions_[session_index].GetSocket());

    sessions_[session_index].SetDisconnected();

    available_session_index_.emplace_back(session_index);

    std::cout << "NETWORK: Close client session " << session_index << std::endl;
}

void TCPServer::Receive(const int session_index) {
    auto & session = sessions_[session_index];

    int received_bytes = session.Receive(MAX_RECV_BUFFER_SIZE);

    if (received_bytes == -1) {
        std::cout << "ERROR: " << error_.GetMessageMap().at(Error::Code::RECEIVE_FAIL) << std::endl;
        CloseSession(session_index);
    } else if (received_bytes == 0) {
        CloseSession(session_index);
    } else {
        std::cout << "NETWORK: Received " << session.GetRecvBuffer() << std::endl;

        unsigned short read_pos = 0;

        auto remain_data = session.GetPrevRecvPos() + received_bytes;

        while (remain_data >= packet_header_size_) {
            // First 2bytes of packet always indicates total size of the packet
            unsigned short packet_size;
            memcpy(&packet_size, &session.GetRecvBuffer()[read_pos], 2);
            auto data = &session.GetRecvBuffer()[read_pos];

            if (remain_data < packet_size) {
                break;
            }

            OnReceive(session_index, data, packet_size);

            read_pos += packet_size;
            remain_data -= packet_size;
        }

        // Move remain data to the first position of receive buffer
        if (remain_data > 0) {
            memcpy(session.GetRecvBuffer(), &session.GetRecvBuffer()[read_pos], remain_data);
        }
        session.SetPrevRecvPos(remain_data);
    }
}

void TCPServer::Send(int session_index, char *data, int size) {
    sessions_[session_index].ClearSendBuffer();

    if (sessions_[session_index].Send(data, size) == Error::Code::SEND_FAIL) {
        std::cout << "ERROR: " << error_.GetMessageMap().at(Error::Code::SEND_FAIL) << std::endl;
        CloseSession(session_index);
    }
}

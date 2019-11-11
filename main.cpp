#include <iostream>
#include <csignal>
#include <functional>
#include "server/chat_server.hpp"

const unsigned short SERVER_PORT = 11021;

std::function<void(int)> server_stop_handler;
void signal_handler(int signal) { server_stop_handler(signal); }

int main() {
    std::signal(SIGINT, signal_handler);

    ChatServer server;

    server_stop_handler = [&](int signal) {
        std::cout << "\nSignal " << signal << " received: server is stopping..." << std::endl;
        server.Stop();
    };

    server.Init(SERVER_PORT);
    server.Start();

    return 0;
}
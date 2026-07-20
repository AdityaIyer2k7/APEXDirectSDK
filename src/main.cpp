#include "transport.hpp"
#include <iostream>
#include <string>

int main() {
    APEXDirectSDK::Gantry::Transport transport;
    std::string ip = "127.0.0.1";
    int port = 1024;
    std::cout << "IP: ";
    std::cin >> ip;
    std::cout << "Port: ";
    std::cin >> port;
    std::cout << ip << " :: " << port << std::endl;
    int ec = transport.connect(ip, port);
    if (ec) std::cout << "Error code: " << ec << std::endl;
    return 0;
}

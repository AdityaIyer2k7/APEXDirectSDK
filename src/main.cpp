#include "transport.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    APEXDirectSDK::Gantry::Transport transport;
    std::string ip = "127.0.0.1";
    int port = 1024;
    if (argc > 0)
        ip = argv[0];
    if (argc > 1)
        port = std::stoi(argv[1]);
    int ec = transport.connect(ip, port);
    std::cout << "Error code: " << ec << std::endl;
    return 0;
}

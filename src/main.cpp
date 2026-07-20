#include "gantry/transport.hpp"
#include <iostream>
#include <string>

using namespace APEXDirectSDK::Gantry;

int main() {
    Transport transport;
    std::string ip = "127.0.0.1";
    int port = 1024;
    std::cout << "IP: ";
    std::cin >> ip;
    std::cout << "Port: ";
    std::cin >> port;
    int ec = transport.connect(ip, port);
    std::cout << "Connecting to " << ip << " :: " << port << std::endl;
    std::cout << "Exit code " << ec << std::endl << std::endl;

    while (true) {
        std::string command;
        std::cout << "Command: ";
        std::cin >> command;
        
        ResponseHandle* rh = new ResponseHandle();
        transport.addCommand(PriorityCommand(command, 0, rh));
        while (!rh->isReady()) {;}

        std::string out;
        rh->read(out);
        std::cout << out << std::endl;
    }
    
    return 0;
}

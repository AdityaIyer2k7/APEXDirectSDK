#include "gantrytransport/transport.hpp"
#include "gantrytransport/parser.hpp"

#include <iostream>
#include <string>

using namespace APEXDirectSDK::Gantry;

void parsetest() {
    std::string test0 = "0 51200.000 >";
    int ec0;     // should be 0
    double val0; // should be 51200.000
    parse(test0, ec0, val0);
    
    std::cout << "EC: " << ec0 << "; ";
    if (!ec0) std::cout << "Value: " << val0 << "; ";
    std::cout << std::endl;
    
    std::string test1 = "1 >";
    int ec1;     // should be 1
    double val1; // will not be read
    parse(test1, ec1, val1);
    
    std::cout << "EC: " << ec1 << "; ";
    if (!ec1) std::cout << "Value: " << val1 << "; ";
    std::cout << std::endl;
}

void telnetloop() {
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
        std::getline(std::cin, command);
        
        ResponseHandle* rh = new ResponseHandle();
        transport.addCommand(PriorityCommand(command, 0, rh));
        while (!rh->isReady()) {;}

        std::string out;
        rh->read(out);
        std::cout << out << std::endl;
        delete rh;
    }
}

int main() {
    parsetest();
    telnetloop();
    
    return 0;
}
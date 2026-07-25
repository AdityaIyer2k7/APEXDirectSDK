#include "gantry/axis.hpp"
#include "gantry/transport.hpp"
#include "gantry/parser.hpp"

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <string>

using namespace APEXDirectSDK::Gantry;

void axistest(Transport& transport) {
    Axis x1(&transport);
    std::string yaml_text =
        "module_idx: 1\n"
        "mm_per_rev: 15\n"
        "inverted: true\n";
    x1.configure(YAML::Load(yaml_text));
    std::cout << "_module_idx: " << x1._module_idx << std::endl;
    std::cout << "_mm_per_rev: " << x1._mm_per_rev << std::endl;
    std::cout << "_inverted: " << x1._inverted << std::endl;
    std::cout << "_home_is_inverted: " << x1._home_is_inverted << std::endl;
}

void parsetest() {
    std::string test0 = "0 51200.000 >";
    int ec0;     // should be 0
    double val0; // should be 51200.000
    parseResponse(test0, ec0, val0);
    
    std::cout << "EC: " << ec0 << "; ";
    if (!ec0) std::cout << "Value: " << val0 << "; ";
    std::cout << std::endl;
    
    std::string test1 = "1 >";
    int ec1;     // should be 1
    double val1; // will not be read
    parseResponse(test1, ec1, val1);
    
    std::cout << "EC: " << ec1 << "; ";
    if (!ec1) std::cout << "Value: " << val1 << "; ";
    std::cout << std::endl;
}

void telnetloop(Transport& transport) {
    std::cout << ">";
    while (true) {
        std::string command;
        std::getline(std::cin, command);
        
        ResponseHandle* rh = new ResponseHandle();
        transport.addCommand(PriorityCommand(command, 0, rh));
        while (!rh->isReady()) {;}

        std::string out;
        rh->read(out);
        std::cout << out;
        delete rh;
    }
}

int main() {
    std::string ip , service;
    std::cout << "IP: ";
    std::getline(std::cin, ip);
    std::cout << "Port: ";
    std::getline(std::cin, service);

    Transport transport;

    std::cout << "Connecting to " << ip << " :: " << service << std::endl;
    int ec = transport.connect(ip, service);
    std::cout << "Exit code " << ec << std::endl << std::endl;

    axistest(transport);
    parsetest();
    telnetloop(transport);
    
    return 0;
}
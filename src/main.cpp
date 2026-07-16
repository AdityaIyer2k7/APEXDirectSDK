#include "transport.hpp"
#include <iostream>


int main() {
    APEXDirectSDK::Gantry::Transport transport;
    int ec = transport.connect("192.168.2.10", 23);
    std::cout << "Error code: " << ec << std::endl;
    return 0;
}

#include "transport.hpp"
#include <iostream>


int main() {
    APEXDirectSDK::Gantry::Transport transport("192.168.2.10", 23);
    int ec = transport.connect();
    std::cout << "Error code: " << ec << std::endl;
    return 0;
}

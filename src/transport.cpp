#include "transport.hpp"

#include <string>

using namespace APEXDirectSDK::Gantry;

bool PriorityCommand::operator<(const PriorityCommand &other) const
{ return this->priority < other.priority; }

Transport::Transport(std::string ip) : Transport(ip, 23) {};

Transport::Transport(std::string ip, int port) {
    inet4_ip = ip;
    inet4_port = port;
    _socket = new ASIO::ip::tcp::socket(_io_ctx);
}

int Transport::connect() {
    try {
        ASIO::ip::tcp::resolver resolver(_io_ctx);
        auto endpoint = resolver.resolve(inet4_ip, inet4_ip);

        ASIO::connect(*_socket, endpoint);
    } catch (std::exception& e) {
        return 1;
    }
    boost::system::error_code ec;
    ASIO::write(*_socket, ASIO::buffer("\r\n"), ec);
    return ec.value();
}

Transport::~Transport() {
    _socket->close();
}

int Transport::addCommand(std::string& command) 
{ return Transport::addCommand({command, 0}); }

int Transport::addCommand(PriorityCommand command) {
    return 0;
}
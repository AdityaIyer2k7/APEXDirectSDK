#include "transport.hpp"

#include <string>

using namespace APEXDirectSDK::Gantry;

bool PriorityCommand::operator<(const PriorityCommand &other) const
{ return this->priority < other.priority; }


Transport::Transport() {}

int Transport::connect(std::string ip)
{ return connect(ip, 23); }

int Transport::connect(std::string ip, int port) {
    if (!_socket.has_value())
        _socket.emplace(_io_ctx);
    else if (_socket.value().is_open())
        return 127;
    
    inet4_ip = ip;
    inet4_port = port;
    
    try {
        ASIO::ip::tcp::resolver resolver(_io_ctx);
        auto endpoint = resolver.resolve(inet4_ip, inet4_ip);

        ASIO::connect(_socket.value(), endpoint);
    } catch (std::exception& e) {
        return 1;
    }
    boost::system::error_code ec;
    ASIO::write(_socket.value(), ASIO::buffer("\r\n"), ec);
    return ec.value();
}

Transport::~Transport() {
    _socket.value().close();
}

int Transport::addCommand(std::string& command) 
{ return Transport::addCommand({command, 0}); }

int Transport::addCommand(PriorityCommand command) {
    return 0;
}
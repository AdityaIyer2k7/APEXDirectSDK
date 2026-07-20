#include "transport.hpp"

#include <iostream>

using namespace APEXDirectSDK::Gantry;

int Transport::connect(std::string ip, std::string service) {
  if (!_socket.has_value()) _socket = boost::asio::ip::tcp::socket(_io_ctx);

  try {
    boost::asio::ip::tcp::resolver resolver(_io_ctx);
    auto endpoint = resolver.resolve(ip, service);

    boost::asio::connect(*_socket, endpoint);
    _ip = ip;
    _port = service;
  } catch (std::exception& e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
    return 1;
  }

  return _send("\r\n");
}

int Transport::addCommand(PriorityCommand command, std::optional<ResponseHandle> &responseHandle) {
  return -1;
}

bool Transport::isConnected() const { return _socket.has_value() && _socket->is_open(); }

int Transport::disconnect() {
  if (!_socket.has_value()) return 127;
  _socket->close();
  _socket = std::nullopt;
  _ip = _port = std::nullopt;
  return 0;
}

Transport::~Transport() {
  disconnect();
}
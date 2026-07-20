#include "generictransport.hpp"
#include <iostream>

using namespace APEXDirectSDK::Gantry;

PriorityCommand::PriorityCommand(std::string* command, int priority)
{
  this->command = command;
  this->priority = priority;
}

bool PriorityCommand::operator<(const PriorityCommand &rhs) const
{
  return this->priority < rhs.priority;
}

bool ResponseHandle::isReady() const { return _ready; }

std::string ResponseHandle::read() {
  std::lock_guard<std::mutex> lock(_mtx);
  if (!_ready) return nullptr;
  return _response;
}

void ResponseHandle::write(std::string data) {
  std::lock_guard<std::mutex> lock(_mtx);
  _response = data;
  _ready = true;
}

int GenericTransport::connect(std::string ip, std::string service) {
  if (!_socket.has_value()) _socket = boost::asio::ip::tcp::socket(_io_ctx);

  try {
    boost::asio::ip::tcp::resolver resolver(_io_ctx);
    auto endpoint = resolver.resolve(ip, service);

    boost::asio::connect(*_socket, endpoint);
    _ip = ip;
    _service = service;
  } catch (std::exception& e) {
    std::cout << "[ERROR] " << e.what() << std::endl;
    return 1;
  }
  return 0;
}

bool GenericTransport::isConnected() const { return _socket.has_value() && _socket->is_open(); }

int GenericTransport::disconnect() {
  if (!_socket.has_value()) return 127;
  _socket->close();
  _socket = std::nullopt;
  _ip = _service = std::nullopt;
  return 0;
}

int GenericTransport::_send(std::string command) {
  if (!_socket.has_value()) return 127;
  boost::system::error_code ec;
  boost::asio::write(*_socket, boost::asio::buffer(command), ec);
  return ec.value();
}

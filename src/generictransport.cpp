#include "generictransport.hpp"

using namespace APEXDirectSDK::Gantry;

bool PriorityCommand::operator<(const PriorityCommand &rhs) const {
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

int GenericTransport::_send(std::string command)
{
  if (!_socket.has_value()) return 127;
  boost::system::error_code ec;
  boost::asio::write(*_socket, boost::asio::buffer(command), ec);
  return ec.value();
}

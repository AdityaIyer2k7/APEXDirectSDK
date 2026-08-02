#include "generictransport.hpp"

#include <iostream>

#include "../errors.h"

using namespace APEXDirectSDK::Gantry;

int GenericTransport::connect(std::string ip, std::string service) {
  if (!_socket.has_value()) _socket = boost::asio::ip::tcp::socket(_io_ctx);

  boost::asio::ip::tcp::resolver resolver(_io_ctx);
  auto endpoint = resolver.resolve(ip, service);

  boost::system::error_code ec;
  boost::asio::connect(*_socket, endpoint, ec);

  if (ec) {
    std::cout << "[ERROR] in GenericTransport::connect;"
      << " wrapped error << "
      << ec.what() << std::endl;
    
    return APEXDirectSDK::Errors::EC_WRAPPED + (ec.value() % APEXDirectSDK::Errors::MUL_MSC_ERR);
  }
  _ip = ip;
  _service = service;

  return 0;
}

bool GenericTransport::isConnected() const
  { return _socket.has_value() && _socket->is_open(); }

int GenericTransport::disconnect() {
  if (!_socket.has_value()) return APEXDirectSDK::Errors::EC_NOTREADY;
  _socket->close();
  _socket = std::nullopt;
  _ip = _service = std::nullopt;
  return 0;
}

int GenericTransport::_send(std::string command) {
  if (!_socket.has_value()) return APEXDirectSDK::Errors::EC_NOTREADY;
  boost::system::error_code ec;
  boost::asio::write(*_socket, boost::asio::buffer(command), ec);
  return ec.value();
}

int GenericTransport::_recv(std::string& out) {
  if (!_socket.has_value()) return APEXDirectSDK::Errors::EC_NOTREADY;

  int waiting = _socket->available();
  if (!waiting) return -127;
  
  boost::asio::streambuf buffer(waiting);
  boost::system::error_code ec;
  boost::asio::read(*_socket, buffer, ec);
  out = std::string(
    boost::asio::buffers_begin(buffer.data()),
    boost::asio::buffers_end(buffer.data())
  );

  if (ec) {
    std::cout << "[ERROR] in GenericTransport::_recv;"
      << " wrapped error << " << ec.what() << std::endl;
    return APEXDirectSDK::Errors::EC_WRAPPED + (ec.value() % APEXDirectSDK::Errors::MUL_MSC_ERR);
  }

  return 0;
}

int GenericTransport::_recv_until(std::string &out, char delimiter) {
  if (!_socket.has_value()) return APEXDirectSDK::Errors::EC_NOTREADY;
  
  boost::asio::streambuf buffer;
  boost::system::error_code ec;
  // boost::asio::read(*_socket, buffer, ec);
  boost::asio::read_until(*_socket, buffer, delimiter, ec);
  out = std::string(
    boost::asio::buffers_begin(buffer.data()),
    boost::asio::buffers_end(buffer.data())
  );
  if (ec) {
    std::cout << "[ERROR] in GenericTransport::_recv_until;"
      << " wrapped error << " << ec.what() << std::endl;
    return APEXDirectSDK::Errors::EC_WRAPPED + (ec.value() % APEXDirectSDK::Errors::MUL_MSC_ERR);
  }

  return 0;
}

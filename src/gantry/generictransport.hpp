#pragma once

#include "boost/asio.hpp"

#include <string>
#include <optional>

namespace APEXDirectSDK::Gantry {
  class GenericTransport {
    public:
      virtual int connect(std::string ip, std::string service);
      virtual bool isConnected() const;
      virtual int disconnect();
      virtual ~GenericTransport() = default;
    protected:
      int _send(std::string command);
      int _recv(std::string& out);
      boost::asio::io_context _io_ctx;
      std::optional<std::string> _ip;
      std::optional<std::string> _service;
      std::optional<boost::asio::ip::tcp::socket> _socket;
  };
}
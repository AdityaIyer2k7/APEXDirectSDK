#pragma once

#include "boost/asio.hpp"

#include <thread>
#include <mutex>
#include <string>
#include <optional>

namespace APEXDirectSDK::Gantry {
  typedef struct PriorityCommand {
    std::string& command;
    int priority;
    bool operator<(const PriorityCommand &rhs) const;
  } PriorityCommand;

  class ResponseHandle {
    bool isReady() const;
    std::string read();
    void write(std::string data);
    private:
      std::mutex _mtx;
      bool _ready = false;
      std::string _response;
  };

  class GenericTransport {
    public:
      virtual int connect(std::string ip, std::string service);
      virtual bool isConnected() const;
      virtual int disconnect();
      virtual int addCommand(PriorityCommand command, std::optional<ResponseHandle>& responseHandle);
      virtual ~GenericTransport() = default;
    protected:
      virtual int _send(std::string command);
      std::string _port;
      std::string _service;
      std::optional<boost::asio::ip::tcp::socket> _socket;
  };
}
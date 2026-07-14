#pragma once

#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "boost/asio.hpp"

#define ASIO boost::asio

namespace APEXDirectSDK::Gantry {
  typedef struct PriorityCommand {
    std::string& command;
    int priority;
    bool operator<(const PriorityCommand &other) const;
  } PriorityCommand;

  class ResponseHandle {
  public:
    ResponseHandle();
    ~ResponseHandle();
    bool isReady() const;
    int read(float& result) const;
  // private:
  };

  class Transport {
  public:
    std::string inet4_ip;
    int inet4_port = 23;
    Transport(std::string ip);
    Transport(std::string ip, int port);
    ~Transport();
    int connect();
    int addCommand(std::string& command);
    int addCommand(PriorityCommand command);
  private:
    ASIO::io_context _io_ctx;
    ASIO::ip::tcp::socket* _socket;
    std::priority_queue<PriorityCommand> _commandsToSend;
  };
}

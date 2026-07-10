#pragma once

#include <queue>
#include <string>

#ifndef NMTCP
#define NMTCP boost::asio::ip::tcp
#endif

namespace APEXDirectSDK::Gantry {
  typedef struct {
    std::string& command;
    int priority;
    bool operator<(const PriorityCommand& lhs, const PriorityCommand& rhs);
  } PriorityCommand;

  class Transport {
  public:
    std::string inet4_ip;
    int inet4_port = 23;
    Transport(std::string ip);
    Transport(std::string ip, int port);
    ~Transport();
    int addCommand(std::string& command);
    int addCommand(PriorityCommand command);
  private:
    NMTCP::socket _socket;
    std::priority_queue<PriorityCommand> _commandsToSend;
  }
}

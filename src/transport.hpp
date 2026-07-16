#pragma once

#include "generictransport.hpp"

#define ASIO boost::asio

namespace APEXDirectSDK::Gantry {
  
  class Transport : public GenericTransport {
  public:
    int connect(std::string ip, std::string service) override;
    int connect(std::string ip, int port)
    { return connect(ip, std::to_string(port)); };

    int addCommand(PriorityCommand command, std::optional<ResponseHandle>& responseHandle) override;
    int addCommand(std::string command, std::optional<ResponseHandle>& responseHandle)
    { return addCommand(PriorityCommand{command, 0}, responseHandle); };

    bool isConnected() const override;

    int disconnect() override;
  private:
    std::optional<std::string> _ip, _port = std::nullopt;
    ASIO::io_context _io_ctx;
  };
}

#pragma once

#include "generictransport.hpp"

#define ASIO boost::asio

namespace APEXDirectSDK::Gantry {
  
  class Transport : GenericTransport {
  public:
  private:
    ASIO::io_context _io_ctx;
  };
}

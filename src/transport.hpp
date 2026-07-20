#pragma once

#include "generictransport.hpp"

namespace APEXDirectSDK::Gantry {
  
  class Transport : public GenericTransport {
  public:
    int connect(std::string ip, std::string service);
    int connect(std::string ip, int service)
    { return connect(ip, std::to_string(service)); };
    
    ~Transport();
  };
}

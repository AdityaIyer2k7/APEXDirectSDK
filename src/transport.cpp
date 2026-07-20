#include "transport.hpp"

#include <iostream>

using namespace APEXDirectSDK::Gantry;

int Transport::connect(std::string ip, std::string service) {
  int ret = GenericTransport::connect(ip, service);
  return ret || _send("\r\n");
}

Transport::~Transport() {
  GenericTransport::disconnect();
}
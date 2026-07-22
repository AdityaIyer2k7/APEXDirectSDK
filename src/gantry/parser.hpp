#pragma once

#include <string>

namespace APEXDirectSDK::Gantry {
  void parseResponse(std::string response, int& ec, double& value);
}
#pragma once

#include <string>

namespace APEXDirectSDK::Gantry {
  void parse(std::string response, int& ec, double& value);
}
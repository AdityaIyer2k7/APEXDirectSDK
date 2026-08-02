#pragma once

#include <string>

#include "yaml-cpp/yaml.h"

namespace APEXDirectSDK::Gantry {
  void parseResponse(std::string response, int& ec, double& value);
  template <typename T> bool getYAMLNodeAs(YAML::Node node, T& out, T defaultValue) {
    out = node ? node.as<T>() : defaultValue;
    return bool(node);
  }
}
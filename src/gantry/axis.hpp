#pragma once

#include "transport.hpp"

#include "yaml-cpp/yaml.h"

namespace APEXDirectSDK::Gantry {
  class Axis {
    public:
      Axis(Transport* transport);
      int configure(YAML::Node axisConfigYAML);
    // private:
      int _send_to_both(std::string command, int priority);
      int _send_to_both(std::string command)
      { return _send_to_both(command, 0); }
      std::string _id_motor() const;
      std::string _id_encoder() const;
      Transport* _transport;
      int _module_idx;
      double _mm_per_rev;
      bool _inverted;
      bool _homed;
      bool _home_is_inverted;
  };
} // namespace APEXDirectSDK::Gantry

#pragma once

#include "transport.hpp"

#include "yaml-cpp/yaml.h"

namespace APEXDirectSDK::Gantry {
  class Axis {
    public:
      Axis(Transport* transport);
      int configure(YAML::Node axisConfigYAML);
    // private: // TODO: Uncommend for public build
      Transport* _transport;
      int _send_to_both(std::string command, int priority);
      std::string _id_motor() const;
      std::string _id_encoder() const;
      int _module_idx;
      double _current;
      double _unit_per_rev;
      double _spd_revps;
      double _acl_revps2;
      bool _inverted;
      bool _home_inverted;
      bool _homed;
  };
} // namespace APEXDirectSDK::Gantry

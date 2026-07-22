#pragma once

#include "yaml-cpp/yaml.h"

namespace APEXDirectSDK::Gantry {
  class Axis {
    public:
      int configure(YAML::Node axisConfig);
    // private:
      int _module_idx;
      double _mm_per_rev;
      bool _inverted;
      bool _homed;
      bool _home_is_inverted;
  };
} // namespace APEXDirectSDK::Gantry

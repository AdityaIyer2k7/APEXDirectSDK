#pragma once

#include "transport.hpp"

#include "yaml-cpp/yaml.h"

namespace APEXDirectSDK::Gantry {
  class Axis {
    public:
      Axis(Transport* transport);
      int configure(YAML::Node axisConfigYAML);
      bool configured() const;
      int setMotor(bool isOn);
      bool getMotor() const;
      int getCurrentLoc(double& encLoc, double& mtrLoc, int priority = 0);
      int setCurrentLoc(double locUnits, int priority = 0);
      int moveTo(double toUnits, int priority = 0);
      int moveBy(double byUnits, int priority = 0);
    // private: // TODO: Uncommend for public build
      Transport* _transport;
      int _send_to_both(std::string command, int priority = 0);
      std::string _id_motor() const;
      std::string _id_encoder() const;
      bool _configured;
      bool _motor_on;
      int _module_idx;
      double _current;
      double _unit_per_rev, _spd_revps, _acl_revps2;
      double _home_loc_rev, _bound_pos_rev, _bound_neg_rev;
      bool _inverted;
      bool _home_negative; // This is *relative to* _inverted; only applies for hardstop homing
      bool _homed;
  };
} // namespace APEXDirectSDK::Gantry

#pragma once

#include "transport.hpp"

#include <yaml-cpp/yaml.h>
#include <pybind11/pybind11.h>

#define PROPERTY(T, prop) \
  private: \
    T _##prop; \
  public: \
    virtual T& get_##prop() { return _##prop; } \
    virtual void set_##prop(T& to) { _##prop = to; }

#define PROP_LIST(MACRO) \
  MACRO(int, module_idx) \
  MACRO(double, current) \
  MACRO(double, unit_per_rev) \
  MACRO(double, spd_revps) \
  MACRO(double, acl_revps2) \
  MACRO(double, home_loc_rev) \
  MACRO(double, bound_pos_rev) \
  MACRO(double, bound_neg_rev) \
  MACRO(bool, inverted) \
  MACRO(bool, home_negative) // This is *relative to* _inverted; only applies for hardstop homing



namespace py = pybind11;

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
      static void bindPybind11(py::module_& m);
    private:
      Transport* _transport;
      int _send_to_both(std::string command, int priority = 0);
      std::string _id_motor() const;
      std::string _id_encoder() const;
      bool _configured;
      bool _motor_on;
      
      PROP_LIST(PROPERTY);
      bool _homed;
  };
} // namespace APEXDirectSDK::Gantry

#undef PROPERTY
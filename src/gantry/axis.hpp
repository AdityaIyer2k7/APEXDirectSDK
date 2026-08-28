#pragma once

#include "transport.hpp"

#include <thread>
#include <atomic>
#include <yaml-cpp/yaml.h>
#include <pybind11/pybind11.h>

#define MAKE_GETTER_SETTER(T, prop) \
  private: \
    T _##prop; \
  public: \
    virtual T& get_##prop() { return _##prop; } \
    virtual void set_##prop(T& to) { _##prop = to; }

#define RUN_MACRO_ON_PROP_LIST(MACRO) \
  MACRO(int, module_idx) \
  MACRO(double, unit_per_rev) \
  MACRO(double, spd_revps) \
  MACRO(double, acl_revps2) \
  MACRO(double, home_loc_rev) \
  MACRO(double, bound_pos_rev) \
  MACRO(double, bound_neg_rev) \
  MACRO(bool, inverted) \
  MACRO(bool, home_negative) /* This is *relative to* _inverted; only applies for hardstop homing */ \



namespace py = pybind11;

namespace APEXDirectSDK::Gantry {
  class Axis {
    public:
      Axis(Transport* transport);
      // Config
      int configure(YAML::Node axisConfigYAML);
      bool configured() const;
      int setMotor(bool isOn, int priority = 0);
      bool getMotor() const;
      int setCurrent(double current, int priority = 0);
      double getCurrent() const;
      // Movement
      int moveTo(double toUnits, int priority = 0);
      int moveBy(double byUnits, int priority = 0);
      int stop(int priority = 127, bool abort = true);
      int fetchCurrentLoc(double& encLocUnits, double& mtrLocUnits, int priority = 0);
      int forceCurrentLoc(double locUnits, int priority = 0);
      // Stall
      int stallMonitorStart(double pollRateHz = 5, int priority = 127);
      int stallMonitorEnd();
      int fetchStalled(bool& stalled);
      bool isStallMonitorRunning();
      static void bindPybind11(py::module_& m);
    private:
      // Config
      Transport* _transport;
      std::string _id_motor() const;
      std::string _id_encoder() const;
      int _send_to_mtr(std::string command, int priority = 0);
      int _send_to_enc(std::string command, int priority = 0);
      int _send_to_both(std::string command, int priority = 0);
      bool _configured = false;
      bool _motor_on = false;
      double _current = 0;
      // Movement
      bool _homed = false;
      int _fetchCurrentLocRaw(double& encLocRevs, double& mtrLocRevs, int priority = 0);
      int _forceCurrentLocRaw(double locRevs, int priority = 0);
      // Stall
      double _stall_poll_rate_hz = 5;
      std::atomic<bool> _stall_thread_running = false;
      std::atomic<bool> _stalled = false;
      std::thread _stall_thread;
      void _stallMonitorLoop(int priority);
      RUN_MACRO_ON_PROP_LIST(MAKE_GETTER_SETTER);
  };
} // namespace APEXDirectSDK::Gantry

#ifndef KEEP_MACROS_axishpp
  #undef MAKE_GETTER_SETTER
  #undef RUN_MACRO_ON_PROP_LIST
#endif

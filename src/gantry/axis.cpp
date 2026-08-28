
#define KEEP_MACROS_axishpp
#include "axis.hpp"

#undef KEEP_MACROS_axishpp

#include "parser.hpp"
#include "../errors.h"

using namespace APEXDirectSDK::Gantry;

Axis::Axis(Transport* transport) {
  _transport = transport;
  _configured = false;
}

int Axis::configure(YAML::Node axisConfigYAML) {
  YAML::Node temp;

  // These settings are required : module#, units/rev, and current
  if (!getYAMLNodeAs<int>(axisConfigYAML["module_idx"], _module_idx, 0) ||
      !getYAMLNodeAs<double>(axisConfigYAML["unit_per_rev"], _unit_per_rev, 0) ||
      !getYAMLNodeAs<double>(axisConfigYAML["current"], _current, 0)
    ) return APEXDirectSDK::Errors::EC_BADINPUT;
  
  if (_unit_per_rev <= 0) return APEXDirectSDK::Errors::EC_BADINPUT;

  /* These settings are open-ended:
   * - encoder counts
   * - speed and acceleration in revolutions per second(^2), 
   * - coordinate inversion
   * - homing direction
   * - bounds relative to home
   */
  int encoder_counts;
  getYAMLNodeAs<int>(axisConfigYAML["encoder_counts"], encoder_counts, 4000);

  getYAMLNodeAs<double>(axisConfigYAML["speed_unit_per_s"], _spd_revps, _unit_per_rev);
  getYAMLNodeAs<double>(axisConfigYAML["accel_unit_per_s2"], _acl_revps2, _unit_per_rev);
  getYAMLNodeAs<double>(axisConfigYAML["home_loc_unit"], _home_loc_rev, 0);
  
  _spd_revps /= _unit_per_rev;
  _acl_revps2 /= _unit_per_rev;
  _home_loc_rev /= _unit_per_rev;

  /* Supports a `bounds` node where:
  * bounds: 180        sets bounds [0, 180]
  * bounds: [180]      sets bounds [0, 180]
  * bounds: [-90, 180] sets bounds [-90, 180]
  */
  YAML::Node bounds = axisConfigYAML["bounds"];
  if (bounds) {
    if (bounds.IsScalar()) _bound_pos_rev = bounds.as<double>() / _unit_per_rev;
    if (bounds.IsSequence()) {
      int bounds_given = bounds.size();
      if (bounds_given >= 1) _bound_pos_rev = bounds[bounds_given - 1].as<double>() / _unit_per_rev;
      if (bounds_given == 2) _bound_neg_rev = bounds[0].as<double>() / _unit_per_rev;
    } 
  }
  
  getYAMLNodeAs<bool>(axisConfigYAML["inverted"], _inverted, false);
  getYAMLNodeAs<bool>(axisConfigYAML["home_negative"], _home_negative, true);

  // Send config commands
  _send_to_mtr("cpu 51200", 130);
  _send_to_enc("cpu " + std::to_string(encoder_counts), 130);
  
  _send_to_both("civ " + std::to_string(_inverted), 129);
  _send_to_both("acp 0", 129);

  _send_to_mtr("amp " + std::to_string(_current), 128);

  _send_to_mtr("spd " + std::to_string(_spd_revps), 128);
  _send_to_mtr("acl " + std::to_string(_acl_revps2), 128);
  _send_to_mtr("dcl " + std::to_string(_acl_revps2), 128);

  _configured = true;
  return 0;
}

bool Axis::configured() const {
  return _configured;
}

int Axis::setMotor(bool isOn, int priority) {
  if (!_configured) return APEXDirectSDK::Errors::EC_NOTREADY;
  
  // int casting is more elegant, but less readable; please don't use it.
  _motor_on = isOn;
  _send_to_mtr((isOn ? "mtr 1" : "mtr 0"), priority);
  
  return 0;
}

bool Axis::getMotor() const {
  return _motor_on;
}

int Axis::setCurrent(double current, int priority) {
  if (!_configured) return APEXDirectSDK::Errors::EC_NOTREADY;

  _current = current;
  return _send_to_mtr("amp " + std::to_string(_current));
}

double Axis::getCurrent() const {
  return _current;
}

int Axis::fetchCurrentLoc(double &encLocUnits, double &mtrLocUnits, int priority) {
  if (!_configured || !_homed) return APEXDirectSDK::Errors::EC_NOTREADY;

  int ec = _fetchCurrentLocRaw(encLocUnits, mtrLocUnits, priority);
  if (ec) return ec;

  encLocUnits *= _unit_per_rev;
  mtrLocUnits *= _unit_per_rev;

  return 0;
}

int Axis::forceCurrentLoc(double locUnits, int priority) {
  if (!_configured) return APEXDirectSDK::Errors::EC_NOTREADY;
  
  return _forceCurrentLocRaw(locUnits / _unit_per_rev, priority);
}

bool Axis::isStallMonitorRunning() {
  return _stall_thread_running;
}

int Axis::fetchStalled(bool &stalled) {
  if (!_configured || !_stall_thread_running) return APEXDirectSDK::Errors::EC_NOTREADY;
  stalled = _stalled;
  return 0;
}

int Axis::_fetchCurrentLocRaw(double &encLocRevs, double &mtrLocRevs, int priority) {
  ResponseHandle encResponse, mtrResponse;

  PriorityCommand encLocCommand(_id_motor() + " acp", priority, &encResponse);
  PriorityCommand mtrLocCommand(_id_encoder() + " acp", priority, &mtrResponse);

  _transport->addCommand(encLocCommand);
  _transport->addCommand(mtrLocCommand);

  while (!encResponse.isReady() || !mtrResponse.isReady()) {;}

  std::string out;
  int ec;
  double value;

  encResponse.read(out);
  parseResponse(out, ec, value);
  if (ec) return APEXDirectSDK::Errors::EC_WRAPPED | ec;
  encLocRevs = value;

  mtrResponse.read(out);
  parseResponse(out, ec, value);
  if (ec) return APEXDirectSDK::Errors::EC_WRAPPED | ec;
  mtrLocRevs = value;
  
  return 0;
}

int Axis::_forceCurrentLocRaw(double locRevs, int priority) { 
  if (locRevs < _bound_neg_rev || locRevs > _bound_pos_rev)
    return APEXDirectSDK::Errors::EC_BADINPUT;
  
  _send_to_both("acp " + std::to_string(locRevs), priority);
  _homed = true;

  return 0;
}

int Axis::moveTo(double toUnits, int priority) {
  if (!_configured || !_motor_on || !_homed) return APEXDirectSDK::Errors::EC_NOTREADY;

  double toRevs = toUnits / _unit_per_rev;

  if (toRevs < _bound_neg_rev || toRevs > _bound_pos_rev)
    return APEXDirectSDK::Errors::EC_BADINPUT;

  _transport->addCommand(_id_motor() + " bmt " + std::to_string(toRevs), priority);

  return 0;
}

int Axis::moveBy(double byUnits, int priority) {
  if (!_configured || !_motor_on) return APEXDirectSDK::Errors::EC_NOTREADY;

  double byRevs = byUnits / _unit_per_rev;

  _transport->addCommand(_id_motor() + " bmb " + std::to_string(byRevs), priority);

  return 0;
}

int Axis::stop(int priority, bool abort) {
  return _send_to_mtr(abort ? "abt" : "stp", priority);
}

int Axis::stallMonitorStart(double pollRateHz, int priority) {
  if (!_configured) return APEXDirectSDK::Errors::EC_NOTREADY;
  if (_stall_thread_running || pollRateHz <= 0.0f) return APEXDirectSDK::Errors::EC_BADINPUT;
  
  _stall_poll_rate_hz = pollRateHz;
  _stall_thread_running = true;
  _stall_thread = std::thread(&Axis::_stallMonitorLoop, this, priority);
  _stall_thread.detach();

  return 0;
}

int Axis::stallMonitorEnd() {
  if (!_stall_thread_running) return APEXDirectSDK::Errors::EC_NOTREADY;

  _stall_thread_running = false;
  return 0;
}

void Axis::_stallMonitorLoop(int priority) {
  const double STALL_TOLERANCE_REV = 0.01;

  auto period = std::chrono::milliseconds(
    static_cast<long long>(1000.0 / _stall_poll_rate_hz));

  while (_stall_thread_running) {
    double encLoc, mtrLoc;
    int ec = _fetchCurrentLocRaw(encLoc, mtrLoc, priority);

    if (!ec) {
      _stalled = std::abs(encLoc - mtrLoc) > STALL_TOLERANCE_REV;
    }

    std::this_thread::sleep_for(period);
  }
}

void Axis::bindPybind11(py::module_ &m) {
  #define BIND_PROPERTY(T, prop) \
    .def_property("_" #prop, &Axis::get_##prop, &Axis::set_##prop)
  
  py::class_<Axis>(m, "Axis")
    .def(py::init<Transport*>())
    .def("configure", [](Axis& axis, std::string yaml_config) {
      return axis.configure(YAML::Load(yaml_config));
    }, py::arg("yaml_config"))
    .def_property_readonly("configured", &Axis::configured)
    .def("setMotor", &Axis::setMotor, py::arg("isOn"), py::arg("priority") = 0)
    .def("getMotor", &Axis::getMotor)
    .def("setCurrent", &Axis::setCurrent, py::arg("current"), py::arg("priority") = 0)
    .def("getCurrent", &Axis::getCurrent)
    .def("moveTo", &Axis::moveTo, py::arg("toUnits"), py::arg("priority") = 0, "Move to commanded units")
    .def("moveBy", &Axis::moveBy, py::arg("byUnits"), py::arg("priority") = 0, "Move by commanded units")
    .def("stop", &Axis::stop, py::arg("priority") = 127, py::arg("abort") = true, "Stop or abort commands")
    .def("fetchCurrentLoc", [](Axis& axis, int priority){
      double encLoc, mtrLoc;
      int ec = axis.fetchCurrentLoc(encLoc, mtrLoc, priority);
      return std::make_tuple(ec, encLoc, mtrLoc);
    }, py::arg("priority") = 0, "Gets current location readings of encoder and motor, returns (ec, encLocUnits, mtrLocUnits)")
    .def("forceCurrentLoc", &Axis::forceCurrentLoc, py::arg("locUnits"), py::arg("priority") = 0, "Forces location of motor and encoder; NOT THE SAME AS MOVEMENT!")
    .def("stallMonitorStart", &Axis::stallMonitorStart, py::arg("pollRateHz") = 5, py::arg("priority") = 0, "Start stall monitoring")
    .def("stallMonitorEnd", &Axis::stallMonitorEnd, "End stall monitoring")
    .def_property_readonly("stalled", [](Axis& axis){
      bool stalled;
      int ec = axis.fetchStalled(stalled);
      return std::make_tuple(ec, stalled);
    }, "Gets the stall state")
    .def_property_readonly("stallMonitorRunning", &Axis::isStallMonitorRunning)
    RUN_MACRO_ON_PROP_LIST(BIND_PROPERTY);
  #undef BIND_PROPERTY
}

int Axis::_send_to_mtr(std::string command, int priority) {
  return _transport->addCommand(_id_motor() + " " + command, priority);
}

int Axis::_send_to_enc(std::string command, int priority) {
  return _transport->addCommand(_id_encoder() + " " + command, priority);
}

int Axis::_send_to_both(std::string command, int priority) {
  return _send_to_mtr(command, priority) | _send_to_enc(command, priority);
}



std::string Axis::_id_motor() const {
  return "a" + std::to_string(2*_module_idx - 1);
}

std::string Axis::_id_encoder() const {
  return "a" + std::to_string(2*_module_idx);
}

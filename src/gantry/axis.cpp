#include "axis.hpp"

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

  /* These settings are open-ended:
   * - encoder counts
   * - speed and acceleration in revolutions per second(^2), 
   * - coordinate inversion
   * - homing direction
   * - bounds relative to home
   * - TODO: home position
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
  _transport->addCommand(_id_motor() + " cpu 51200", 130);
  _transport->addCommand(_id_encoder() + " cpu " + std::to_string(encoder_counts), 130);
  
  _transport->addCommand(_id_motor() + " amp " + std::to_string(_current), 130);

  _send_to_both("civ " + std::to_string(_inverted), 129);
  _send_to_both("acp 0", 128);

  _configured = true;
  return 0;
}

bool Axis::configured() const {
  return _configured;
}

int Axis::setMotor(bool isOn) {
  if (!_configured) return APEXDirectSDK::Errors::EC_NOTREADY;
  
  // int casting is more elegant, but less readable; please don't use it.
  _motor_on = isOn;
  _transport->addCommand(_id_motor() + (isOn ? " mtr 1" : " mtr 0"), 128);
  
  return 0;
}

bool Axis::getMotor() const {
  return _motor_on;
}

int Axis::getCurrentLoc(double &encLoc, double &mtrLoc, int priority) {
  if (!_configured || !_homed) return APEXDirectSDK::Errors::EC_NOTREADY;
  
  ResponseHandle encResponse, mtrResponse;

  PriorityCommand encLocCommand(_id_encoder() + " acp", priority, &encResponse);
  PriorityCommand mtrLocCommand(_id_motor() + " acp", priority, &mtrResponse);

  _transport->addCommand(encLocCommand);
  _transport->addCommand(mtrLocCommand);

  while (!encResponse.isReady() || !mtrResponse.isReady()) {;}

  std::string out;
  int ec;
  double value;

  encResponse.read(out);
  parseResponse(out, ec, value);
  if (ec) return APEXDirectSDK::Errors::EC_WRAPPED | ec;
  encLoc = value;

  mtrResponse.read(out);
  parseResponse(out, ec, value);
  if (ec) return APEXDirectSDK::Errors::EC_WRAPPED | ec;
  mtrLoc = value;
  
  return 0;
}

int Axis::setCurrentLoc(double locUnits, int priority) {
  if (!_configured) return APEXDirectSDK::Errors::EC_NOTREADY;
  
  double locRevs = locUnits / _unit_per_rev;

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

void Axis::bindPybind11(py::module_ &m) {
  #define BIND_PROPERTY(T, prop) \
    .def_property("_" #prop, &Axis::get_##prop, &Axis::set_##prop)
  
  py::class_<Axis>(m, "Axis")
    .def(py::init<Transport*>())
    .def("configure", &Axis::configure)
    .def_property_readonly("configured", &Axis::configured)
    .def("getMotor", &Axis::getMotor)
    .def("setMotor", &Axis::setMotor, py::arg("isOn"))
    .def("getCurrentLoc", [](Axis& axis, int priority){
      double encLoc, mtrLoc;
      int ec = axis.getCurrentLoc(encLoc, mtrLoc, priority);
      return std::make_tuple(ec, encLoc, mtrLoc);
    }, py::arg("priority"), "Gets current location readings of encoder and motor, returns (ec, encLoc, mtrLoc)")
    .def("setCurrentLoc", &Axis::setCurrentLoc, py::arg("locUnits"), py::arg("priority"), "Sets apparent current location of motor and encoder; NOT THE SAME AS MOVEMENT!")
    .def("moveTo", &Axis::moveTo, py::arg("toUnits"), py::arg("priority"), "Move to commanded units")
    .def("moveBy", &Axis::moveBy, py::arg("byUnits"), py::arg("priority"), "Move by commanded units")
    PROP_LIST(BIND_PROPERTY);
  #undef BIND_PROPERTY
}

int Axis::_send_to_both(std::string command, int priority) {
  int ec_mtr = _transport->addCommand(_id_motor() + " " + command, priority);
  int ec_enc = _transport->addCommand(_id_encoder() + " " + command, priority);
  return ec_mtr | ec_enc;
}



std::string Axis::_id_motor() const {
  return "a" + std::to_string(2*_module_idx - 1);
}

std::string Axis::_id_encoder() const {
  return "a" + std::to_string(2*_module_idx);
}




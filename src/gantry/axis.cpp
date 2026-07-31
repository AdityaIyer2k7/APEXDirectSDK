#include "axis.hpp"

#include "parser.hpp"

using namespace APEXDirectSDK::Gantry;

Axis::Axis(Transport* transport) {
  _transport = transport;
}

int Axis::configure(YAML::Node axisConfigYAML) {
  YAML::Node temp;

  if (!getYAMLNodeAs<int>(axisConfigYAML["module_idx"], _module_idx, 0)) return 127;
  if (!getYAMLNodeAs<double>(axisConfigYAML["unit_per_rev"], _unit_per_rev, 0)) return 127;
  if (!getYAMLNodeAs<double>(axisConfigYAML["current"], _current, 0)) return 127;

  int encoder_counts;
  getYAMLNodeAs<int>(axisConfigYAML["encoder_counts"], encoder_counts, 4000);

  getYAMLNodeAs<double>(axisConfigYAML["speed_unit_per_s"], _spd_revps, _unit_per_rev);
  getYAMLNodeAs<double>(axisConfigYAML["accel_unit_per_s2"], _acl_revps2, _unit_per_rev);
  _spd_revps /= _unit_per_rev;
  _acl_revps2 /= _unit_per_rev;
  
  getYAMLNodeAs<bool>(axisConfigYAML["inverted"], _inverted, false);
  getYAMLNodeAs<bool>(axisConfigYAML["home_inverted"], _home_inverted, false);

  _transport->addCommand(_id_motor() + " cpu 51200", 2);
  _transport->addCommand(_id_encoder() + " cpu " + std::to_string(encoder_counts), 2);

  _send_to_both("civ " + std::to_string(_inverted), 1);
  _send_to_both("acp 0", 0);

  return 0;
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

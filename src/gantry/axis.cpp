#include "axis.hpp"

#include "parser.hpp"

using namespace APEXDirectSDK::Gantry;

Axis::Axis(Transport* transport) {
  _transport = transport;
}

int Axis::configure(YAML::Node axisConfigYAML) {
  YAML::Node temp;

  if (temp = axisConfigYAML["module_idx"]) _module_idx = temp.as<int>();
  else return 127;
  if (temp = axisConfigYAML["mm_per_rev"]) _mm_per_rev = temp.as<double>();
  else return 127;
  int encoder_counts = (temp = axisConfigYAML["encoder_counts"]) ? temp.as<int>() : 4000;
  _inverted = (temp = axisConfigYAML["inverted"]) ? temp.as<bool>() : false;
  _home_is_inverted = (temp = axisConfigYAML["home_inverted"]) ? temp.as<bool>() : false;

  _transport->addCommand(_id_motor() + " cpu 51200", 2);
  _transport->addCommand(_id_encoder() + " cpu " + std::to_string(encoder_counts), 2);

  _send_to_both("civ " + std::to_string(_inverted), 1);
  _send_to_both("acp 0");

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

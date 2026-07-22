#include "axis.hpp"

using namespace APEXDirectSDK::Gantry;

int Axis::configure(YAML::Node axisConfig) {
  YAML::Node temp;
  
  if (temp = axisConfig["module_idx"]) _module_idx = temp.as<int>(); else return 127;
  if (temp = axisConfig["mm_per_rev"]) _mm_per_rev = temp.as<double>(); else return 127;
  _inverted = (temp = axisConfig["inverted"]) ? temp.as<bool>() : false;
  _home_is_inverted = (temp = axisConfig["home_inverted"]) ? temp.as<bool>() : false;

  return 0;
}
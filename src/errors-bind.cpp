#include "errors-bind.hpp"

namespace py = pybind11;

void APEXDirectSDK::Errors::bind(py::module_ m) {
  m.attr("J_USR_ERR") = APEXDirectSDK::Errors::J_USR_ERR;
  m.attr("J_MSC_ERR") = APEXDirectSDK::Errors::J_MSC_ERR;

  m.attr("MUL_USR_ERR") = APEXDirectSDK::Errors::MUL_USR_ERR;
  m.attr("MUL_MSC_ERR") = APEXDirectSDK::Errors::MUL_MSC_ERR;
  
  m.attr("EC_GOOD") = APEXDirectSDK::Errors::EC_GOOD;
  m.attr("EC_BADINPUT") = APEXDirectSDK::Errors::EC_BADINPUT;
  m.attr("EC_NOTREADY") = APEXDirectSDK::Errors::EC_NOTREADY;
  m.attr("EC_WRAPPED") = APEXDirectSDK::Errors::EC_WRAPPED;
  m.attr("EC_UNIMPLEMENTED") = APEXDirectSDK::Errors::EC_UNIMPLEMENTED;
}
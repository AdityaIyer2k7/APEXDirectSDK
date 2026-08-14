#include <pybind11/pybind11.h>

#include "errors-bind.hpp"
#include "gantry/gantry-bind.hpp"

namespace py = pybind11;

PYBIND11_MODULE(APEXDirectPySDK, m, py::mod_gil_not_used()) {
  py::module_ subm_errors = m.def_submodule("errors");
  py::module_ subm_gantry = m.def_submodule("gantry");

  APEXDirectSDK::Errors::bind(subm_errors);
  APEXDirectSDK::Gantry::bind(subm_gantry);
}
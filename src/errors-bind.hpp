#pragma once

#include <pybind11/pybind11.h>
#include "errors.h"

namespace py = pybind11;

namespace APEXDirectSDK::Errors {
  void bind(py::module_ m);
}
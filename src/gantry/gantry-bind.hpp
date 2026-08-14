#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include "parser.hpp"
#include "generictransport.hpp"
#include "transport.hpp"
#include "axis.hpp"

namespace py = pybind11;

namespace APEXDirectSDK::Gantry
{
    void bind_parser(py::module_ m);
    void bind_generictransport(py::module_ m);
    void bind_transport(py::module_ m);
    void bind_axis(py::module_ m);
    
    void bind(py::module_ m);
} // namespace APEXDirectSDK::Gantry

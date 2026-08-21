#include "gantry-bind.hpp"

namespace py = pybind11;

using namespace APEXDirectSDK;

void Gantry::bind_parser(py::module_& m) {
  m.def("parseResponse", [](std::string response) {
      int ec; double value;
      parseResponse(response, ec, value);
      return std::make_tuple(ec, value);
    },
    "Parse responses from ModuSystems units in the form \"[EC] [VALUE] >\"\n"
    "e.g. \"0 152.000 >\" returns tuple (0, 152.0)"
    );
}

void Gantry::bind_generictransport(py::module_& m) {
  py::class_<GenericTransport>(m, "GenericTransport")
    .def(py::init<>())
    .def("connect", &GenericTransport::connect,
      "Connect to specified ip & service",
      py::arg("ip"), py::arg("service"))
    .def("disconnect", &GenericTransport::disconnect)
    .def_property_readonly("isConnected", &GenericTransport::isConnected);
}

void Gantry::bind_transport(py::module_& m) {
  py::class_<ResponseHandle, std::shared_ptr<ResponseHandle>>(m, "Response")
    .def(py::init<>())
    .def("read", [](ResponseHandle& rh){
      std::string out;
      int ec = rh.read(out);
      return std::make_tuple(ec, out);
    })
    .def("write", &ResponseHandle::write, py::arg("data"))
    .def_property_readonly("isReady", &ResponseHandle::isReady);
  
  py::class_<PriorityCommand>(m, "PriorityCommand")
    .def(py::init<std::string, int, ResponseHandle*>(),
      py::arg("command"), py::arg("priority"),
      py::arg("responseHandle") = nullptr
    )
    .def_readwrite("command", &PriorityCommand::command)
    .def_readwrite("priority", &PriorityCommand::priority)
    .def_readwrite("responseHandle", &PriorityCommand::responseHandle)
    .def(py::self < py::self);
  
  py::class_<Transport, GenericTransport>(m, "Transport")
    .def(py::init<>())
    .def("connect", py::overload_cast<std::string, std::string>(&Transport::connect))
    .def("connect", py::overload_cast<std::string, int>(&Transport::connect))
    .def("addCommand", py::overload_cast<PriorityCommand>(&Transport::addCommand), py::arg("pc"))
    .def("addCommand", py::overload_cast<std::string, int>(&Transport::addCommand), py::arg("command"), py::arg("priority")=0)
    ;
}

void Gantry::bind_axis(py::module_& m) {
  Axis::bindPybind11(m);
}



void Gantry::bind(py::module_& m) {
  Gantry::bind_parser(m);
  Gantry::bind_generictransport(m);
  Gantry::bind_transport(m);
  Gantry::bind_axis(m);
}


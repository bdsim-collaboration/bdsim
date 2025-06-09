#include <pybind11/pybind11.h>

#include "BDSException.hh"

namespace py = pybind11;

PYBIND11_MODULE(bdsexception, m) {
  py::register_exception<BDSException>(m, "BDSException", PyExc_RuntimeError);
}
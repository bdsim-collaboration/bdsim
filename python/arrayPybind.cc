#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "array.h"
#include "sym_table.h"

PYBIND11_MODULE(array, m) {
  py::class_<GMAD::Array>(m,"Array")
    .def(py::init<>());
}
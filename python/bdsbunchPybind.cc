//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSBunch.hh"

PYBIND11_MODULE(bdsbunch, m) {
  py::class_<BDSBunch>(m, "BDSBunch")
    .def(py::init<std::string &>());
}
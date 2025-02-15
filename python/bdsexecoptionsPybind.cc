//
// Created by Stewart Boogert on 15/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSExecOptions.hh"

PYBIND11_MODULE(bdsexecoptions, m) {
py::class_<BDSExecOptions>(m,"BDSExecOptions");
}

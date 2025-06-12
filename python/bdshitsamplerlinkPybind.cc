//
// Created by Stewart Boogert on 15/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <string>
#include <vector>

#include "BDSHitSamplerLink.hh"

PYBIND11_MODULE(bdshitsamplerlink, m) {
  py::class_<BDSHitSamplerLink>(m,"BDSHitSamplerLink");
}
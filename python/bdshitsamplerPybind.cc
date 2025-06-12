//
// Created by Stewart Boogert on 15/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <string>
#include <vector>

#include "BDSHitSampler.hh"

PYBIND11_MODULE(bdshitsampler, m) {
  py::class_<BDSHitSampler>(m,"BDSHitSampler");
}
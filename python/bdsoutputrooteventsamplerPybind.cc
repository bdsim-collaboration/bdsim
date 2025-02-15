//
// Created by Stewart Boogert on 24/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "BDSOutputROOTEventSampler.hh"

PYBIND11_MODULE(bdsoutputrooteventsampler, m) {
  py::class_<BDSOutputROOTEventSampler<double>>(m,"BDSOutputROOTEventSampler_double");

  py::class_<BDSOutputROOTEventSampler<float>>(m,"BDSOutputROOTEventSampler_float");
};
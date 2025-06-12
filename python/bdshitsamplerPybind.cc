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
  py::class_<BDSHitSampler>(m,"BDSHitSampler")
    .def_readonly("samplerID", &BDSHitSampler::samplerID)
    .def_readonly("coords", &BDSHitSampler::coords)
    .def_readonly("momentum", &BDSHitSampler::momentum)
    .def_readonly("mass", &BDSHitSampler::mass)
    .def_readonly("charge", &BDSHitSampler::charge)
    .def_readonly("rigidity", &BDSHitSampler::rigidity)
    .def_readonly("pdgID", &BDSHitSampler::pdgID)
    .def_readonly("parentID", &BDSHitSampler::parentID)
    .def_readonly("trackID", &BDSHitSampler::trackID)
    .def_readonly("turnsTaken", &BDSHitSampler::turnsTaken)
    .def_readonly("beamlineIndex", &BDSHitSampler::beamlineIndex)
    .def_readonly("nElectrons", &BDSHitSampler::nElectrons);
}
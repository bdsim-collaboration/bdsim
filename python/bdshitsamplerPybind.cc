/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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
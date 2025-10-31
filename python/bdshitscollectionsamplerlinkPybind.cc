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

#include "BDSOutput.hh"
#include "G4THitsCollection.hh"
#include "BDSHitSamplerLink.hh"

PYBIND11_MODULE(bdshitscollectionsamplerlink, m) {
  py::class_<BDSHitsCollectionSamplerLink>(m,"BDSHitsCollectionSamplerLink")
    .def("PrintAllHits", &BDSHitsCollectionSamplerLink::PrintAllHits)
    .def("__getitem__", [](const BDSHitsCollectionSamplerLink &self, size_t i) {
      return self[i];
    }, py::return_value_policy::reference_internal)
    .def("entries", &BDSHitsCollectionSamplerLink::entries);
}
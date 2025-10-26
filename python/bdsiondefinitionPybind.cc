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
// Created by Stewart Boogert on 07/06/2025.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <string>
#include <vector>
#include <sstream>

#include "BDSIonDefinition.hh"

PYBIND11_MODULE(bdsiondefinition, m) {
  py::class_<BDSIonDefinition>(m, "BDSIonDefinition")
    .def(py::init<std::string>())
    .def(py::init<G4int, G4double, G4double>())
    .def("A",&BDSIonDefinition::A)
    .def("Z",&BDSIonDefinition::Z)
    .def("Charge",&BDSIonDefinition::Charge)
    .def("ExcitationEnergy",&BDSIonDefinition::ExcitationEnergy)
    .def("OverrideCharge",&BDSIonDefinition::OverrideCharge)
    .def("NElectrons",&BDSIonDefinition::NElectrons)
    .def("__str__", [](const BDSIonDefinition &self) {
       std::ostringstream oss;
       oss << self;
       return oss.str();
     });
}
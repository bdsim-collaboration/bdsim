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

#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoords.hh"

PYBIND11_MODULE(bdsparticlecoordsfull, m) {
  py::class_<BDSParticleCoordsFull, BDSParticleCoords>(m,"BDSParticleCoordsFull")
    .def(py::init<>())
    .def(py::init<G4double,  G4double,  G4double,
                  G4double, G4double, G4double,
                  G4double,  G4double,
                  G4double, G4double>(),
                  py::arg("x"),
                  py::arg("y"),
                  py::arg("z"),
                  py::arg("xp"),
                  py::arg("yp"),
                  py::arg("zp"),
                  py::arg("t"),
                  py::arg("s"),
                  py::arg("totalEnergy"),
                  py::arg("weight"))
    .def(py::init<BDSParticleCoords &, G4double, G4double, G4double>(),
         py::arg("local"),
         py::arg("s"),
         py::arg("totalEnergy"),
         py::arg("weight"))
     .def("__str__", [](const BDSParticleCoordsFull &self)
     {
       std::ostringstream oss;
       oss << self;
       return oss.str();
     })
     .def_readonly("s", &BDSParticleCoordsFull::s)
     .def_readonly("totalEnergy", &BDSParticleCoordsFull::totalEnergy)
     .def_readonly("weight", &BDSParticleCoordsFull::weight);

}
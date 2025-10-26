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

#include "BDSParticleCoords.hh"

PYBIND11_MODULE(bdsparticlecoords, m) {
  py::class_<BDSParticleCoords>(m, "BDSParticleCoords")
    .def(py::init<>())
    .def(py::init<G4double, G4double, G4double,
                  G4double, G4double, G4double,
                  G4double>(),
         py::arg("x"),
         py::arg("y"),
         py::arg("z"),
         py::arg("xp"),
         py::arg("yp"),
         py::arg("zp"),
         py::arg("t"))
    .def(py::init<>([](py::list pos,
                       py::list mom,
                       G4double t) {
      return new BDSParticleCoords(G4ThreeVector(pos[0].cast<double>(), pos[1].cast<double>(), pos[2].cast<double>()),
                                   G4ThreeVector(mom[0].cast<double>(), mom[1].cast<double>(), mom[2].cast<double>()),
                                   t);
    }))
    .def("__str__", [](const BDSParticleCoords &self)
    {
      std::ostringstream oss;
      oss << self;
      return oss.str();
     })
    .def("Position", [](const BDSParticleCoords &pc) {
      auto pv = pc.Position();
      py::list retval;
      retval.append(pv[0]);
      retval.append(pv[1]);
      retval.append(pv[2]);
      return retval;
    })
    .def("Momentum", [](const BDSParticleCoords &pc) {
      auto mv = pc.Momentum();
      py::list retval;
      retval.append(mv[0]);
      retval.append(mv[1]);
      retval.append(mv[2]);
      return retval;
    })
    .def_readonly("x", &BDSParticleCoords::x)
    .def_readonly("y", &BDSParticleCoords::y)
    .def_readonly("z", &BDSParticleCoords::z)
    .def_readonly("xp", &BDSParticleCoords::xp)
    .def_readonly("yp", &BDSParticleCoords::yp)
    .def_readonly("zp", &BDSParticleCoords::zp)
    .def_readonly("T", &BDSParticleCoords::T);
}
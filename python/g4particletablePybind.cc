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

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

PYBIND11_MODULE(g4particletable, m) {
  py::class_<G4ParticleTable>(m, "G4ParticleTable")
    .def_static("GetParticleTable", &G4ParticleTable::GetParticleTable, py::return_value_policy::reference)
    .def("contains", [](G4ParticleTable *pt, G4ParticleDefinition *pd) { return pt->contains(pd);})
    .def("contains", [](G4ParticleTable *pt, std::string name) { return pt->contains(name);})
    .def("entries", &G4ParticleTable::entries)
    .def("size", &G4ParticleTable::size)
    .def("FindParticle", [](G4ParticleTable *pt, G4int pdg) {return pt->FindParticle(pdg);},
         py::return_value_policy::reference_internal)
    .def("DumpTable", [](G4ParticleTable *pt, std::string particle) {pt->DumpTable(particle);},
         py::arg("particle") = "ALL")
    .def("GetIonTable", &G4ParticleTable::GetIonTable, py::return_value_policy::reference_internal)
    .def("RemoveAllParticles", &G4ParticleTable::RemoveAllParticles)
    .def("DeleteAllParticles", &G4ParticleTable::DeleteAllParticles)
    .def("SelectParticle", [](G4ParticleTable *pt, std::string name)
      {
        pt->SelectParticle(name);
      })
    .def("GetSelectedParticle", &G4ParticleTable::GetSelectedParticle);

}
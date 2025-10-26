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

#include "G4String.hh"
#include "G4IonTable.hh"

PYBIND11_MODULE(g4iontable, m) {
  py::class_<G4IonTable>(m, "G4IonTable")
    .def_static("GetIonTable", &G4IonTable::GetIonTable, py::return_value_policy::reference)
    .def("GetNumberOfElements", &G4IonTable::GetNumberOfElements)
    .def("CreateAllIon", &G4IonTable::CreateAllIon)
    .def("CreateAllIsomer", &G4IonTable::CreateAllIsomer)
    .def("PreparNuclideTable", &G4IonTable::PrepareNuclideTable)
    .def("PreloadNuclide", &G4IonTable::PreloadNuclide)
    .def("GetIon",[](G4IonTable *it, G4int Z, G4int A, G4int lvl) {return it->GetIon(Z,A,lvl);},
         py::arg("Z"),
         py::arg("A"),
         py::arg("lvl") = 0,
         py::return_value_policy::reference_internal)
    .def("GetIon", [](G4IonTable *it, G4int pdg) { return it->GetIon(pdg); },
         py::arg("pdf"),
         py::return_value_policy::reference_internal)
    .def("Entries", &G4IonTable::Entries)
    .def("GetIonMass", &G4IonTable::GetIonMass,
         py::arg("Z"),
         py::arg("A"),
         py::arg("nL") = 0,
         py::arg("lvl") = 0)
    .def("DumpTable", [](G4IonTable *pt, std::string particle) {pt->DumpTable(particle);},
         py::arg("particle") = "ALL");
}
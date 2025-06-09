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
#include "G4ParticleDefinition.hh"
#include "G4DecayTable.hh"

PYBIND11_MODULE(g4particledefinition, m) {
  py::class_<G4ParticleDefinition>(m, "G4ParticleDefinition")
    .def(py::init<const G4String&,
                  G4double, G4double, G4double,
                  G4int, G4int, G4int, G4int,
                  G4int, G4int, const G4String&, G4int,
                  G4int, G4int, G4bool, G4double,
                  G4DecayTable*, G4bool,
                  const G4String&, G4int,
                  G4double>(),
         py::arg("aName"), py::arg("mass"), py::arg("width"), py::arg("charge"),
         py::arg("iSpin"), py::arg("iParity"), py::arg("iConjugation"), py::arg("iIsospin"),
         py::arg("iIsospinZ"), py::arg("gParity"), py::arg("pType"), py::arg("lepton"),
         py::arg("baryon"), py::arg("encoding"), py::arg("stable"), py::arg("lifetime"),
         py::arg("decaytable"), py::arg("shortlived") = false,
         py::arg("subType") = "", py::arg("anti_encoding") = 0,
         py::arg("magneticMoment") = 0.0)
    .def("__eq__", &G4ParticleDefinition::operator==)
    .def("__ne__", &G4ParticleDefinition::operator!=);
}
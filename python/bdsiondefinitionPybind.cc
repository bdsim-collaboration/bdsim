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
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

#include "G4ParticleDefinition.hh"
#include "BDSParticleDefinition.hh"
#include "BDSIonDefinition.hh"

PYBIND11_MODULE(bdsparticledefinition, m) {
    py::class_<BDSParticleDefinition>(m, "BDSParticleDefinition")
      .def(py::init<G4ParticleDefinition *, G4double, G4double, G4double, G4double, BDSIonDefinition*, G4int>(),
                    py::arg("particleIn"),
                    py::arg("totalEnergyIn"),
                    py::arg("kineticEnergyIn"),
                    py::arg("momentumIn"),
                    py::arg("ffact"),
                    py::arg("ionDefinitionIn") = nullptr,
                    py::arg("ionPDGID") = 0)
      .def(py::init<const std::string&, G4double, G4double, G4double,
                    G4double, G4double, G4double,
                    BDSIonDefinition*, G4int>(),
                    py::arg("nameIn"),
             py::arg("massIn"),
             py::arg("chargeIn"),
             py::arg("totalEnergyIn"),
             py::arg("kineticEnergyIn"),
             py::arg("momentumIn"),
             py::arg("ffact"),
             py::arg("ionDefinitionIn") = nullptr,
             py::arg("ionPDGID") = 0)
      .def(py::init<BDSParticleDefinition&>())
      .def(py::init<const BDSParticleDefinition&>())
      .def("SetEnergies", &BDSParticleDefinition::SetEnergies)
      .def("UpdateG4ParticleDefinition",&BDSParticleDefinition::UpdateG4ParticleDefinition)
      .def("ApplyChangeInKineticEnergy", &BDSParticleDefinition::ApplyChangeInKineticEnergy)
      .def("ParticleDefinition", &BDSParticleDefinition::ParticleDefinition)
      .def("IonDefinition", &BDSParticleDefinition::IonDefinition)
      .def("Name", [](BDSParticleDefinition *part)
        {
          return std::string(part->Name());
        })
      .def("Mass", &BDSParticleDefinition::Mass)
      .def("Charge", &BDSParticleDefinition::Charge)
      .def("TotalEnergy", &BDSParticleDefinition::TotalEnergy)
      .def("KineticEnergy", &BDSParticleDefinition::KineticEnergy)
      .def("Momentum", &BDSParticleDefinition::Momentum)
      .def("Gamma", &BDSParticleDefinition::Gamma)
      .def("Beta", &BDSParticleDefinition::Beta)
      .def("BRho", &BDSParticleDefinition::BRho)
      .def("FFact", &BDSParticleDefinition::FFact)
      .def("IsAnIon", &BDSParticleDefinition::IsAnIon)
      .def("NElectrons", &BDSParticleDefinition::NElectrons)
      .def("Velocity", &BDSParticleDefinition::Velocity)
      .def("Forwards", &BDSParticleDefinition::Forwards)
      .def("PDGID", &BDSParticleDefinition::PDGID)
      .def("__str__", [](const BDSParticleDefinition &self)
        {
           std::ostringstream oss;
           oss << self;
           return oss.str();
        });
}
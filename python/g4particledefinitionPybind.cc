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
#include "G4ProcessManager.hh"
#if G4VERSION_NUMBER >= 41100
#include "G4VTrackingManager.hh"
#endif
#include "G4ParticleTable.hh"

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
    .def("__ne__", &G4ParticleDefinition::operator!=)
    .def("GetParticleName", [](G4ParticleDefinition *pd) {return std::string(pd->GetParticleName());})
    .def("GetPDGMass", &G4ParticleDefinition::GetPDGMass)
    .def("GetPDGWidth", &G4ParticleDefinition::GetPDGWidth)
    .def("GetPDGCharge", &G4ParticleDefinition::GetPDGCharge)
    .def("GetPDGSpin", &G4ParticleDefinition::GetPDGSpin)
    .def("GetPDGiSpin", &G4ParticleDefinition::GetPDGiSpin)
    .def("GetPDGiParity", &G4ParticleDefinition::GetPDGiParity)
    .def("GetPDGiConjugation", &G4ParticleDefinition::GetPDGiConjugation)
    .def("GetPDGIsospin", &G4ParticleDefinition::GetPDGIsospin)
    .def("GetPDGIsospin3", &G4ParticleDefinition::GetPDGIsospin3)
    .def("GetPDGiIsospin", &G4ParticleDefinition::GetPDGiIsospin)
    .def("GetPDGiIsospin3", &G4ParticleDefinition::GetPDGiIsospin3)
    .def("GetPDGiGParity", &G4ParticleDefinition::GetPDGiGParity)
    .def("GetPDGMagneticMoment", &G4ParticleDefinition::GetPDGMagneticMoment)
    .def("SetPDGMagneticMoment", &G4ParticleDefinition::SetPDGMagneticMoment)
    .def("CalculateAnomaly", &G4ParticleDefinition::CalculateAnomaly)
    .def("GetParticleType", [](G4ParticleDefinition *pd) {std::string(pd->GetParticleType());})
    .def("GetParticleSubType", [](G4ParticleDefinition *pd) {std::string(pd->GetParticleSubType());})
    .def("GetLeptonNumber", &G4ParticleDefinition::GetLeptonNumber)
    .def("GetBaryonNumber", &G4ParticleDefinition::GetBaryonNumber)
    .def("GetPDGEncoding", &G4ParticleDefinition::GetPDGEncoding)
    .def("GetAntiPDGEncoding", &G4ParticleDefinition::GetAntiPDGEncoding)
    .def("SetAntiPDGEncoding", &G4ParticleDefinition::SetAntiPDGEncoding)
    .def("GetQuarkContent", &G4ParticleDefinition::GetQuarkContent)
    .def("GetAntiQuarkContent", &G4ParticleDefinition::GetAntiQuarkContent)
    .def("IsShortLived", &G4ParticleDefinition::IsShortLived)
    .def("GetPDGStable", &G4ParticleDefinition::GetPDGStable)
    .def("SetPDGStable", &G4ParticleDefinition::SetPDGStable)
    .def("GetPDGLifeTime", &G4ParticleDefinition::GetPDGLifeTime)
    .def("SetPDGLifeTime", &G4ParticleDefinition::SetPDGLifeTime)
    .def("GetIonLifeTime", &G4ParticleDefinition::GetIonLifeTime)
    .def("GetDecayTable", &G4ParticleDefinition::GetDecayTable)
    .def("SetDecayTable", &G4ParticleDefinition::SetDecayTable)
    .def("GetProcessManager", &G4ParticleDefinition::GetProcessManager)
    .def("SetProcessManager", &G4ParticleDefinition::GetProcessManager)
#if G4VERSION_NUMBER >= 41100
    .def("GetTrackingManager", &G4ParticleDefinition::GetTrackingManager)
    .def("SetTrackingManager", &G4ParticleDefinition::GetTrackingManager)
#endif
    .def("GetParticleTable", &G4ParticleDefinition::GetParticleTable)
    .def("GetAtomicNumber", &G4ParticleDefinition::GetAtomicNumber)
    .def("GetAtomicMass", &G4ParticleDefinition::GetAtomicMass)
    .def("DumpTable", &G4ParticleDefinition::DumpTable);
}
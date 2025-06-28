//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSBunch.hh"
#include "BDSBunchLink.hh"

PYBIND11_MODULE(bdsbunchlink, m) {
  py::class_<BDSBunchLink, BDSBunch>(m, "BDSBunchLink")
    .def(py::init<>())
    .def("GetNextParticleLocal", &BDSBunchLink::GetNextParticleLocal)
    .def("AddParticle", &BDSBunchLink::AddParticle, py::keep_alive<1, 2>())
    .def("ClearParticles", &BDSBunchLink::ClearParticles)
    .def("Size", &BDSBunchLink::Size)
    .def("CurrentExternalParticleID", &BDSBunchLink::CurrentExternalParticleID)
    .def("CurrentExternalParentID", &BDSBunchLink::CurrentExternalParentID)
    .def("ParticleDefinition", &BDSBunchLink::ParticleDefinition)
    .def("UpdateGeant4ParticleDefinition", &BDSBunchLink::UpdateGeant4ParticleDefinition)
    .def("UpdateIonDefinition", &BDSBunchLink::UpdateIonDefinition);
}
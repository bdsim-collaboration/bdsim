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
#include "BDSLinkBunch.hh"

PYBIND11_MODULE(bdslinkbunch, m) {
  py::class_<BDSLinkBunch, BDSBunch>(m, "BDSLinkBunch")
    .def(py::init<>())
    .def("GetNextParticleLocal", &BDSLinkBunch::GetNextParticleLocal)
    .def("AddParticle", &BDSLinkBunch::AddParticle, py::keep_alive<1, 2>())
    .def("ClearParticles", &BDSLinkBunch::ClearParticles)
    .def("Size", &BDSLinkBunch::Size)
    .def("CurrentExternalParticleID", &BDSLinkBunch::CurrentExternalParticleID)
    .def("CurrentExternalParentID", &BDSLinkBunch::CurrentExternalParentID)
    .def("ParticleDefinition", &BDSLinkBunch::ParticleDefinition)
    .def("UpdateGeant4ParticleDefinition", &BDSLinkBunch::UpdateGeant4ParticleDefinition)
    .def("UpdateIonDefinition", &BDSLinkBunch::UpdateIonDefinition);
}
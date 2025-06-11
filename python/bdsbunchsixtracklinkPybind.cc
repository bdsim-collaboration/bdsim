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
#include "BDSBunchSixTrackLink.hh"

PYBIND11_MODULE(bdsbunchsixtracklink, m) {
  py::class_<BDSBunchSixTrackLink, BDSBunch>(m, "BDSBunchSixTrackLink")
    .def(py::init<>())
    .def("GetNextParticleLocal", &BDSBunchSixTrackLink::GetNextParticleLocal)
    .def("AddParticle", &BDSBunchSixTrackLink::AddParticle, py::keep_alive<1, 2>())
    .def("ClearParticles", &BDSBunchSixTrackLink::ClearParticles)
    .def("Size", &BDSBunchSixTrackLink::Size)
    .def("CurrentExternalParticleID", &BDSBunchSixTrackLink::CurrentExternalParticleID)
    .def("CurrentExternalParentID", &BDSBunchSixTrackLink::CurrentExternalParentID)
    .def("ParticleDefinition", &BDSBunchSixTrackLink::ParticleDefinition)
    .def("UpdateGeant4ParticleDefinition", &BDSBunchSixTrackLink::UpdateGeant4ParticleDefinition)
    .def("UpdateIonDefinition", &BDSBunchSixTrackLink::UpdateIonDefinition);
}
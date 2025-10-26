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
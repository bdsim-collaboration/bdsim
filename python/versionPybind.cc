/*
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

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
// Created by Stewart Boogert on 3/10/2026.
//
#include <string>

#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(version, m) {
  m.attr("BDSIM_VERSION") = "@BDSIM_VERSION@";
  m.attr("BDSIM_MAJOR_VERSION") = "@BDSIM_MAJOR_VERSION@";
  m.attr("BDSIM_MINOR_VERSION") = "@BDSIM_MINOR_VERSION@";
  m.attr("BDSIM_PATCH_VERSION") = "@BDSIM_PATCH_LEVEL@";
  m.attr("VERSION_SHA1") = "@VERSION_SHA1@";
  m.attr("ROOT_VERSION") = "@ROOT_VERSION@";
  m.attr("CLHEP_VERSION") = "@CLHEP_VERSION@";
  m.attr("HEPMC3_VERSION") = "@HepMC3_VERSION@";
  m.attr("HDF5_VERSION") = "@HDF5_VERSION@";
}
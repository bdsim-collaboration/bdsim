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
// Created by Stewart Boogert on 15/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <string>
#include <vector>

#include "BDSIMClass.hh"
#include "BDSParser.hh"

PYBIND11_MODULE(bdsimclass, m) {
py::class_<BDSIM>(m,"BDSIM")
    .def(py::init<>())
    .def(py::init<BDSParser *>())
    .def(py::init<>(
        [](std::vector<std::string> args, bool usualPrintOut=true) {
            std::vector<char *> cstrs;
            cstrs.reserve(args.size());
            for (auto &s: args) cstrs.push_back(const_cast<char *>(s.c_str()));
            return new BDSIM(args.size(), cstrs.data(), usualPrintOut);
        }))
    .def("Initialised",&BDSIM::Initialised)
    .def("InitialisationResult",&BDSIM::InitialisationResult)
    .def("BeamOn",&BDSIM::BeamOn);
}
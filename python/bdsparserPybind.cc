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

#include "BDSParser.hh"

PYBIND11_MODULE(bdsparser, m) {
  py::class_<BDSParser>(m,"BDSParser")
    .def(py::init<>())
    .def_static("Instance",[](std::string fileName) {return BDSParser::Instance(fileName);})// ,py::return_value_policy::reference)
    .def_static("Instance",[]() {return BDSParser::Instance();})
    .def_static("IsInitialised",&BDSParser::IsInitialised)
    .def("GetOptions", [](BDSParser &bdsp) {return &bdsp.GetOptions();}, py::return_value_policy::reference)
    .def("GetBeam",[](BDSParser &bdsp) {return &bdsp.GetBeam();}, py::return_value_policy::reference)
    .def("GetBeamline", [](BDSParser &bdsp) {return &bdsp.GetBeamline();}, py::return_value_policy::reference)
    .def("GetSamplerFilterIDToSet",[](BDSParser &bdsp) {return bdsp.GetSamplerFilterIDToSet();}, py::return_value_policy::reference)
    .def("GetSequence",&BDSParser::GetSequence)
    .def("GetElement", [](BDSParser &bdsp, const std::string& name) {
      std::cout << name << std::endl;
      return bdsp.GetElement(name);})
    .def("PrintBeamline", &BDSParser::PrintBeamline)
    .def("PrintElements", &BDSParser::PrintElements)
    .def("PrintOptions", &BDSParser::PrintOptions);
}

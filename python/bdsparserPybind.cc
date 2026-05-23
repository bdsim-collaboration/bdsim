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
  py::class_<BDSParser, GMAD::Parser>(m,"BDSParser")
    .def(py::init<>())
    .def_static("Instance",[](std::string fileName) {return BDSParser::Instance(fileName);})// ,py::return_value_policy::reference)
    .def_static("Instance",[]() {return BDSParser::Instance();})
    .def_static("IsInitialised",&BDSParser::IsInitialised)
    .def("AmalgamateOptions", &BDSParser::AmalgamateOptions)
    .def("AmgalgamateBeam", &BDSParser::AmalgamateBeam)
    .def("CheckOptions", &BDSParser::CheckOptions)

    .def("GetApertures", &BDSParser::GetApertures)
    .def("GetAtoms", &BDSParser::GetAtoms)
    .def("GetBeam",[](BDSParser &bdsp) {return &bdsp.GetBeam();}, py::return_value_policy::reference_internal)
    .def("GetBeamBase", &BDSParser::GetBeamBase, py::return_value_policy::reference_internal)
    .def("GetBeamline", [](BDSParser &bdsp) {return &bdsp.GetBeamline();}, py::return_value_policy::reference_internal)
    .def("GetBiasing",[](BDSParser &bdsp) {return &bdsp.GetBiasing();}, py::return_value_policy::reference_internal)
    .def("GetBiasingVector",&BDSParser::GetBiasingVector, py::return_value_policy::reference_internal)
    .def("GetBLMs",&BDSParser::GetBLMs, py::return_value_policy::reference_internal)
    .def("GetCavityModels", &BDSParser::GetCavityModels,  py::return_value_policy::reference_internal)
    .def("GetColours", &BDSParser::GetColours,  py::return_value_policy::reference_internal)
    .def("GetCoolingChannels", &BDSParser::GetCoolingChannels, py::return_value_policy::reference_internal)
    .def("GetCoolingChannel",[](BDSParser &bdsp,const std::string& name) {return bdsp.GetCoolingChannel(name);} , py::return_value_policy::reference_internal)
    .def("GetCrystals", &BDSParser::GetCrystals, py::return_value_policy::reference_internal)
    .def("GetElement", [](BDSParser &bdsp, const std::string& name) { return bdsp.GetElement(name);})
    .def("GetFields", &BDSParser::GetFields,py::return_value_policy::reference_internal)
    .def("GetOptions", [](BDSParser &bdsp) {return &bdsp.GetOptions();}, py::return_value_policy::reference_internal)
    .def("GetOptionsBase", &BDSParser::GetBeamBase, py::return_value_policy::reference_internal)
    .def("GetLasers", &BDSParser::GetLasers,py::return_value_policy::reference_internal)
    .def("GetMaterials", &BDSParser::GetMaterials,py::return_value_policy::reference_internal)
    .def("GetModulators", &BDSParser::GetModulators, py::return_value_policy::reference_internal)
    .def("GetPlacements", &BDSParser::GetPlacements, py::return_value_policy::reference_internal)
    .def("GetPlacementElements", [](BDSParser &bdsp, const std::string& name) {return bdsp.GetPlacementElement(name);})
    .def("GetQueries", &BDSParser::GetQueries, py::return_value_policy::reference_internal)
    .def("GetRegions", &BDSParser::GetRegions, py::return_value_policy::reference_internal)
    .def("GetSamplerFilterIDToSet",[](BDSParser &bdsp) {return bdsp.GetSamplerFilterIDToSet();}, py::return_value_policy::reference_internal)
    .def("GetScorers", &BDSParser::GetScorers, py::return_value_policy::reference_internal)
    .def("GetScorerMeshes", &BDSParser::GetScorerMeshes, py::return_value_policy::reference_internal)
    .def("GetSequence", &BDSParser::GetSequence)
    .def("GetTunnels", &BDSParser::GetTunnels)
    .def("PrintBeamline", &BDSParser::PrintBeamline)
    .def("PrintElements", &BDSParser::PrintElements)
    .def("PrintOptions", &BDSParser::PrintOptions);
}

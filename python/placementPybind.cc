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
// Created by Stewart Boogert on 18/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "placement.h"
#include "samplerplacement.h"
#include "scorermesh.h"
#include "blmplacement.h"
#include "query.h"

PYBIND11_MODULE(placement, m) {
  py::class_<GMAD::Published<GMAD::Placement>>(m,"PublishedPlacement")
    .def("NameExists", &GMAD::Placement::NameExists)
    .def("AllNames", &GMAD::Placement::AllNames);

  py::class_<GMAD::Placement, GMAD::Published<GMAD::Placement>>(m,"Placement")
    .def(py::init<>())
    .def(py::init<const GMAD::SamplerPlacement &>())
    .def(py::init<const GMAD::ScorerMesh &>())
    .def(py::init<const GMAD::BLMPlacement&>())
    .def(py::init<const GMAD::Query&>())

    .def("clear",&GMAD::Placement::clear)
    .def("print",&GMAD::Placement::print)

    .def_readonly("name",&GMAD::Placement::name)
    .def_readonly("geometryFile",&GMAD::Placement::geometryFile)
    .def_readonly("bdsimElement",&GMAD::Placement::bdsimElement)
    .def_readonly("sequence",&GMAD::Placement::sequence)
    .def_readonly("referenceElement",&GMAD::Placement::referenceElement)
    .def_readonly("referenceElementNumber",&GMAD::Placement::referenceElementNumber)
    .def_readonly("s",&GMAD::Placement::s)
    .def_readonly("x",&GMAD::Placement::x)
    .def_readonly("y",&GMAD::Placement::y)
    .def_readonly("z",&GMAD::Placement::z)

    .def_readonly("phi",&GMAD::Placement::phi)
    .def_readonly("theta",&GMAD::Placement::theta)
    .def_readonly("psi",&GMAD::Placement::psi)

    .def_readonly("axisX",&GMAD::Placement::axisX)
    .def_readonly("axisY",&GMAD::Placement::axisY)
    .def_readonly("axisZ",&GMAD::Placement::axisZ)
    .def_readonly("angle",&GMAD::Placement::angle)

    .def_readonly("sensitive",&GMAD::Placement::sensitive)
    .def_readonly("axisAngle",&GMAD::Placement::axisAngle)
    .def_readonly("side",&GMAD::Placement::side)
    .def_readonly("sideOffset",&GMAD::Placement::sideOffset)
    .def_readonly("autoColour",&GMAD::Placement::autoColour)
    .def_readonly("stripOuterVolume",&GMAD::Placement::stripOuterVolume)
    .def_readonly("dontReloadGeometry",&GMAD::Placement::dontReloadGeometry)

    .def_readonly("fieldAll", &GMAD::Placement::fieldAll)

    .def("set_value",[](GMAD::Placement &self,std::string name,bool value) {self.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Placement &self,std::string name,int value) {self.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Placement &self,std::string name,long int value) {self.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Placement &self,std::string name,double value) {self.set_value<double>(name,value);})
    .def("set_value",[](GMAD::Placement &self,std::string name,std::string value) {self.set_value<std::string>(name,value);})

    .def("keys", [](GMAD::Placement &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Placement &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Placement &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Placement &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Placement &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Placement &self) {return self.AllNames();});

}

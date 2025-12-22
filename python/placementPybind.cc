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

    .def_readwrite("name",&GMAD::Placement::name)
    .def_readwrite("geometryFile",&GMAD::Placement::geometryFile)
    .def_readwrite("bdsimElement",&GMAD::Placement::bdsimElement)
    .def_readwrite("sequence",&GMAD::Placement::sequence)
    .def_readwrite("referenceElement",&GMAD::Placement::referenceElement)
    .def_readwrite("referenceElementNumber",&GMAD::Placement::referenceElementNumber)
    .def_readwrite("s",&GMAD::Placement::s)
    .def_readwrite("x",&GMAD::Placement::x)
    .def_readwrite("y",&GMAD::Placement::y)
    .def_readwrite("z",&GMAD::Placement::z)

    .def_readwrite("phi",&GMAD::Placement::phi)
    .def_readwrite("theta",&GMAD::Placement::theta)
    .def_readwrite("psi",&GMAD::Placement::psi)

    .def_readwrite("axisX",&GMAD::Placement::axisX)
    .def_readwrite("axisY",&GMAD::Placement::axisY)
    .def_readwrite("axisZ",&GMAD::Placement::axisZ)
    .def_readwrite("angle",&GMAD::Placement::angle)

    .def_readwrite("sensitive",&GMAD::Placement::sensitive)
    .def_readwrite("axisAngle",&GMAD::Placement::axisAngle)
    .def_readwrite("side",&GMAD::Placement::side)
    .def_readwrite("sideOffset",&GMAD::Placement::sideOffset)
    .def_readwrite("autoColour",&GMAD::Placement::autoColour)
    .def_readwrite("stripOuterVolume",&GMAD::Placement::stripOuterVolume)
    .def_readwrite("dontReloadGeometry",&GMAD::Placement::dontReloadGeometry)

    .def_readwrite("fieldAll", &GMAD::Placement::fieldAll)

    .def("set_value",[](GMAD::Placement &placement,std::string name,std::string value) {placement.set_value<std::string>(name,value);})
    .def("set_value",[](GMAD::Placement &placement,std::string name,int value) {placement.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Placement &placement,std::string name,bool value) {placement.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Placement &placement,std::string name,long int value) {placement.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Placement &placement,std::string name,double value) {placement.set_value<double>(name,value);})

    .def("keys", [](GMAD::Placement &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Placement &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Placement &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Placement &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Placement &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Placement &self) {return self.AllNames();});

}

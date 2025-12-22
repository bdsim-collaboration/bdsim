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
// Created by Stewart Boogert on 12/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "blmplacement.h"

PYBIND11_MODULE(blmplacement, m) {
  py::class_<GMAD::Published<GMAD::BLMPlacement>>(m,
  "PublishedBLMPlacement")
    .def("NameExists",&GMAD::BLMPlacement::NameExists)
    .def("AllNames", &GMAD::BLMPlacement::AllNames);

  py::class_<GMAD::BLMPlacement, GMAD::Published<GMAD::BLMPlacement> >(m,"BLMPlacement")
    .def(py::init<>())
    .def("clear", &GMAD::BLMPlacement::clear)
    .def("print", &GMAD::BLMPlacement::print)

    .def_readonly("name", &GMAD::BLMPlacement::name)
    .def_readonly("referenceElement", &GMAD::BLMPlacement::referenceElement)
    .def_readonly("referenceElementNumber", &GMAD::BLMPlacement::referenceElementNumber)

    .def_readonly("s", &GMAD::BLMPlacement::s)
    .def_readonly("x", &GMAD::BLMPlacement::x)
    .def_readonly("y", &GMAD::BLMPlacement::y)
    .def_readonly("z", &GMAD::BLMPlacement::z)

    .def_readonly("phi", &GMAD::BLMPlacement::phi)
    .def_readonly("theta", &GMAD::BLMPlacement::theta)
    .def_readonly("psi", &GMAD::BLMPlacement::psi)

    .def_readonly("axisX", &GMAD::BLMPlacement::axisX)
    .def_readonly("axisY", &GMAD::BLMPlacement::axisY)
    .def_readonly("axisZ", &GMAD::BLMPlacement::axisZ)
    .def_readonly("angle", &GMAD::BLMPlacement::angle)

    .def_readonly("axisAngle", &GMAD::BLMPlacement::axisAngle)
    .def_readonly("side", &GMAD::BLMPlacement::side)
    .def_readonly("sideOffset", &GMAD::BLMPlacement::sideOffset)

    .def_readonly("geometryFile", &GMAD::BLMPlacement::geometryFile)
    .def_readonly("geometryType", &GMAD::BLMPlacement::geometryType)
    .def_readonly("blmMaterial", &GMAD::BLMPlacement::blmMaterial)
    .def_readonly("blm1", &GMAD::BLMPlacement::blm1)
    .def_readonly("blm2", &GMAD::BLMPlacement::blm2)
    .def_readonly("blm3", &GMAD::BLMPlacement::blm3)
    .def_readonly("blm4", &GMAD::BLMPlacement::blm4)
    .def_readonly("scoreQuantity", &GMAD::BLMPlacement::scoreQuantity)
    .def_readonly("bias", &GMAD::BLMPlacement::bias)

    .def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,std::string value) {blmp.set_value<std::string>(name,value);})
    .def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,int value) {blmp.set_value<int>(name,value);})
    .def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,bool value) {blmp.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,long int value) {blmp.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,double value) {blmp.set_value<double>(name,value);})

    .def("keys", [](GMAD::BLMPlacement &self) {return self.AllNames();})
    .def("__len__", [](GMAD::BLMPlacement &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::BLMPlacement &self, const std::string& key, int value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::BLMPlacement &self, const std::string& key, double value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::BLMPlacement &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::BLMPlacement &self) {return self.AllNames();});
;

}

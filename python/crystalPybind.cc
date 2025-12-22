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
// Created by Stewart Boogert on 16/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "crystal.h"

PYBIND11_MODULE(crystal, m) {
  py::class_<GMAD::Published<GMAD::Crystal>>(m,"PublishedCrystal")
  .def("NameExists",&GMAD::Crystal::NameExists)
  .def("AllNames", &GMAD::Crystal::AllNames);

  py::class_<GMAD::Crystal, GMAD::Published<GMAD::Crystal>>(m,"Crystal")
  .def(py::init<>())

  .def("clear",&GMAD::Crystal::clear)
  .def("print",&GMAD::Crystal::print)

  .def_readonly("name", &GMAD::Crystal::name)
  .def_readonly("material", &GMAD::Crystal::material)
  .def_readonly("data", &GMAD::Crystal::data)
  .def_readonly("shape", &GMAD::Crystal::shape)
  .def_readonly("lengthX", &GMAD::Crystal::lengthX)
  .def_readonly("lengthY", &GMAD::Crystal::lengthY)
  .def_readonly("lengthZ", &GMAD::Crystal::lengthZ)
  .def_readonly("sizeA", &GMAD::Crystal::sizeA)
  .def_readonly("sizeB", &GMAD::Crystal::sizeB)
  .def_readonly("sizeC", &GMAD::Crystal::sizeC)
  .def_readonly("alpha", &GMAD::Crystal::alpha)
  .def_readonly("beta", &GMAD::Crystal::beta)
  .def_readonly("gamma", &GMAD::Crystal::gamma)
  .def_readonly("spaceGroup", &GMAD::Crystal::spaceGroup)
  .def_readonly("bendingAngleYAxis", &GMAD::Crystal::bendingAngleYAxis)
  .def_readonly("bendingAngleZAxis", &GMAD::Crystal::bendingAngleZAxis)
  .def_readonly("miscutAngleY", &GMAD::Crystal::miscutAngleY)

  .def("set_value",[](GMAD::Crystal &crystal,std::string name,std::string value) {crystal.set_value<std::string>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,int value) {crystal.set_value<int>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,bool value) {crystal.set_value<bool>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,long int value) {crystal.set_value<long int>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,double value) {crystal.set_value<double>(name,value);})

  .def("keys", [](GMAD::Crystal &self) {return self.AllNames();})
  .def("__len__", [](GMAD::Crystal &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::Crystal &self, const std::string& key, int value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Crystal &self, const std::string& key, double value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Crystal &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Crystal &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::Crystal &self) {return self.AllNames();});
}
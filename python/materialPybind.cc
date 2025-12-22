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

#include "material.h"

PYBIND11_MODULE(material, m) {
  py::class_<GMAD::Published<GMAD::Material>>(m,"PublishedMaterial")
  .def("NameExists", &GMAD::Material::NameExists)
  .def("AllNames", &GMAD::Material::AllNames);

  py::class_<GMAD::Material, GMAD::Published<GMAD::Material>>(m,"Material")
  .def(py::init<>())
  .def("clear",&GMAD::Material::clear)
  .def("print",&GMAD::Material::print)

  .def_readonly("A",&GMAD::Material::A)
  .def_readonly("Z",&GMAD::Material::Z)
  .def_readonly("density",&GMAD::Material::density)
  .def_readonly("temper",&GMAD::Material::temper)
  .def_readonly("pressure",&GMAD::Material::pressure)
  .def_readonly("state", &GMAD::Material::state)
  .def_readonly("components", &GMAD::Material::components)
  .def_readonly("componentsFractions", &GMAD::Material::componentsFractions)
  .def_readonly("componentsWeights", &GMAD::Material::componentsWeights)

  .def("set_value",[](GMAD::Material &self,std::string name,bool value) {self.set_value<bool>(name,value, false);})
  .def("set_value",[](GMAD::Material &self,std::string name,int value) {self.set_value<int>(name,value, false);})
  .def("set_value",[](GMAD::Material &self,std::string name,long int value) {self.set_value<long int>(name,value, false);})
  .def("set_value",[](GMAD::Material &self,std::string name,double value) {self.set_value<double>(name,value, false);})
  .def("set_value",[](GMAD::Material &self,std::string name,GMAD::Array *value) {self.set_value(name,value, false);})
  .def("set_value",[](GMAD::Material &self,std::string name,std::string value) {self.set_value<std::string>(name,value, false);})

  .def("keys", [](GMAD::Material &self) {return self.AllNames();})
  .def("__len__", [](GMAD::Material &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::Material &self, const std::string& key, int value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Material &self, const std::string& key, double value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Material &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Material &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::Material &self) {return self.AllNames();});
}
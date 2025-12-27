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
// Created by Stewart Boogert on 19/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "parameters.h"
#include "elementtype.h"

PYBIND11_MODULE(parameters, m) {
  py::class_<GMAD::Parameters, GMAD::Element, GMAD::Published<GMAD::Element> >(m,"Parameters")
    .def(py::init<>())
    .def_readonly("setMap",&GMAD::Parameters::setMap)
    .def("flush",&GMAD::Parameters::flush)
    .def("inherit_properties",&GMAD::Parameters::inherit_properties)

    .def("set_value",[](GMAD::Parameters &parameters,std::string name,std::string value) {parameters.set_value<std::string>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,int value) {parameters.set_value<int>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,GMAD::ElementType value) {parameters.set_value<GMAD::ElementType>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,bool value) {parameters.set_value<bool>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,long int value) {parameters.set_value<long int>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,double value) {parameters.set_value<double>(name,value, false);})
    .def("set_value",[](GMAD::Parameters& self, std::string name, GMAD::Array *value) {self.set_value_array(name,value,true);})


    .def("keys", [](GMAD::Parameters &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Parameters &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Parameters &self, const std::string& key, bool value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Parameters &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Parameters &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Parameters &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::Parameters &self, const std::string& key, GMAD::Array *value) {self.set_value_array(key, value, false);})
    .def("__setitem__", [](GMAD::Parameters &self, const std::string& key, py::list &value) {
      py::module_ m = py::module_::import("bdsim");
      py::object cls = m.attr("Array");  // get the class
      py::object obj = cls(value);       // call constructor
      auto array = obj.cast<GMAD::Array*>();
      self.set_value_array(key, array, false);
    })
    .def("_ipython_key_completions_", [](GMAD::Parameters &self) {return self.AllNames();});
}
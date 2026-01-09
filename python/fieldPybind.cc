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

#include "field.h"

PYBIND11_MODULE(field, m) {
  py::class_<GMAD::Published<GMAD::Field>>(m,"PublishedField")
  .def("NameExists",&GMAD::Field::NameExists)
  .def("AllNames", &GMAD::Field::AllNames);

  py::class_<GMAD::Field, GMAD::Published<GMAD::Field>>(m,"Field")
  .def(py::init<>())

  .def("clear",&GMAD::Field::clear)
  .def("print",&GMAD::Field::print)

  .def_readonly("name", &GMAD::Field::name)
  .def_readonly("type", &GMAD::Field::type)
  .def_readonly("eScaling", &GMAD::Field::eScaling)
  .def_readonly("bScaling", &GMAD::Field::bScaling)
  .def_readonly("integrator", &GMAD::Field::integrator)
  .def_readonly("globalTransform", &GMAD::Field::globalTransform)
  .def_readonly("magneticFile", &GMAD::Field::magneticFile)
  .def_readonly("magneticInterpolator", &GMAD::Field::magneticInterpolator)
  .def_readonly("electricFile", &GMAD::Field::electricFile)
  .def_readonly("electricInterpolator", &GMAD::Field::electricInterpolator)
  .def_readonly("fieldModulator", &GMAD::Field::fieldModulator)

  .def_readonly("x", &GMAD::Field::x)
  .def_readonly("y", &GMAD::Field::y)
  .def_readonly("z", &GMAD::Field::z)
  .def_readonly("t", &GMAD::Field::t)
  .def_readonly("phi", &GMAD::Field::phi)
  .def_readonly("theta", &GMAD::Field::theta)
  .def_readonly("psi", &GMAD::Field::psi)
  .def_readonly("axisX", &GMAD::Field::axisX)
  .def_readonly("axisY", &GMAD::Field::axisY)
  .def_readonly("axisZ", &GMAD::Field::axisZ)
  .def_readonly("angle", &GMAD::Field::angle)
  .def_readonly("axisAngle", &GMAD::Field::axisAngle)
  .def_readonly("autoScale", &GMAD::Field::autoScale)
  .def_readonly("maximumStepLength", &GMAD::Field::maximumStepLength)
  .def_readonly("magneticSubField", &GMAD::Field::magneticSubField)
  .def_readonly("electricSubField", &GMAD::Field::electricSubField)
  .def_readonly("magneticReflection", &GMAD::Field::magneticReflection)
  .def_readonly("electricReflection", &GMAD::Field::electricReflection)
  .def_readonly("fieldParameters", &GMAD::Field::fieldParameters)

  .def("set_value",[](GMAD::Field &self,std::string name,bool value) {self.set_value<bool>(name,value, false);})
  .def("set_value",[](GMAD::Field &self,std::string name,int value) {self.set_value<int>(name,value, false);})
  .def("set_value",[](GMAD::Field &self,std::string name,long int value) {self.set_value<long int>(name,value, false);})
  .def("set_value",[](GMAD::Field &self,std::string name,double value) {self.set_value<double>(name,value, false);})
  .def("set_value",[](GMAD::Field &self,std::string name,std::string value) {self.set_value<std::string>(name,value, false);})
  .def("get_value",[](GMAD::Field &self,std::string name) {
    std::variant<bool, int, double, std::string, py::list> retval;

    try {
      retval = self.get<bool>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    try {
      retval = self.get<int>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    try {
      retval = self.get<double>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    try {
      retval = self.get<std::string>(&self,name);
      return retval;
    }
    catch (const std::runtime_error&) {}

    throw std::runtime_error("name not found : "+name);
  })

  .def("keys", [](GMAD::Field &self) {return self.AllNames();})
  .def("__len__", [](GMAD::Field &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::Field &self, const std::string& key, bool value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Field &self, const std::string& key, int value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Field &self, const std::string& key, double value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::Field &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::Field &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::Field &self) {return self.AllNames();});
}

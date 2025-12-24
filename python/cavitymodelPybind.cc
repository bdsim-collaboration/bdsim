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

#include "cavitymodel.h"

PYBIND11_MODULE(cavitymodel, m) {
  py::class_<GMAD::Published<GMAD::CavityModel>>(m,"PublishedCavityModel")
  .def("NameExists",&GMAD::CavityModel::NameExists)
  .def("AllNames", &GMAD::CavityModel::AllNames);

  py::class_<GMAD::CavityModel, GMAD::Published<GMAD::CavityModel>>(m,"CavityModel")
  .def(py::init<>())
  .def("clear",&GMAD::CavityModel::clear)
  .def("print", &GMAD::CavityModel::print)

  .def_readonly("name", &GMAD::CavityModel::name)
  .def_readonly("type", &GMAD::CavityModel::type)
  .def_readonly("material", &GMAD::CavityModel::material)
  .def_readonly("irisRadius", &GMAD::CavityModel::irisRadius)
  .def_readonly("equatorRadius", &GMAD::CavityModel::equatorRadius)
  .def_readonly("halfCellLength", &GMAD::CavityModel::halfCellLength)
  .def_readonly("equatorHorizontalAxis", &GMAD::CavityModel::equatorHorizontalAxis)
  .def_readonly("equatorVerticalAxis", &GMAD::CavityModel::equatorVerticalAxis)
  .def_readonly("irisHorizontalAxis",&GMAD::CavityModel::irisHorizontalAxis)
  .def_readonly("irisVerticalAxis", &GMAD::CavityModel::irisVerticalAxis)
  .def_readonly("tangetLineAngle", &GMAD::CavityModel::tangentLineAngle)
  .def_readonly("thickness", &GMAD::CavityModel::thickness)
  .def_readonly("numberOfPoints", &GMAD::CavityModel::numberOfPoints)
  .def_readonly("numberOfCells", &GMAD::CavityModel::numberOfCells)

  .def("set_value",[](GMAD::CavityModel &self,std::string name,bool value) {self.set_value<bool>(name,value);})
  .def("set_value",[](GMAD::CavityModel &self,std::string name,int value) {self.set_value<int>(name,value);})
  .def("set_value",[](GMAD::CavityModel &self,std::string name,long int value) {self.set_value<long int>(name,value);})
  .def("set_value",[](GMAD::CavityModel &self,std::string name,double value) {self.set_value<double>(name,value);})
  .def("set_value",[](GMAD::CavityModel &self,std::string name,std::string value) {self.set_value<std::string>(name,value);})
  .def("get_value",[](GMAD::CavityModel &self,std::string name) {
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

  .def("keys", [](GMAD::CavityModel &self) {return self.AllNames();})
  .def("__len__", [](GMAD::CavityModel &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::CavityModel &self, const std::string& key, int value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::CavityModel &self, const std::string& key, double value) {self.set_value(key, value, false);})
  .def("__setitem__", [](GMAD::CavityModel &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::CavityModel &self) {return self.AllNames();});
}
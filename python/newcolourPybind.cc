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
// Created by Stewart Boogert on 17/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "newcolour.h"

PYBIND11_MODULE(newcolour, m) {
  py::class_<GMAD::Published<GMAD::NewColour>>(m,"PublishedNewColour")
  .def("NameExists", &GMAD::NewColour::NameExists)
  .def("AllNames", &GMAD::NewColour::AllNames);

  py::class_<GMAD::NewColour, GMAD::Published<GMAD::NewColour>>(m,"NewColour")
  .def(py::init<>())
  .def("clear",&GMAD::NewColour::clear)
  .def("print",&GMAD::NewColour::print)

  .def_readonly("name",&GMAD::NewColour::name)
  .def_readonly("red",&GMAD::NewColour::red)
  .def_readonly("green",&GMAD::NewColour::green)
  .def_readonly("blue",&GMAD::NewColour::blue)
  .def_readonly("alpha",&GMAD::NewColour::alpha)

  .def("set_value",[](GMAD::NewColour &newcolour,std::string name,std::string value) {newcolour.set_value<std::string>(name,value, false);})
  .def("set_value",[](GMAD::NewColour &newcolour,std::string name,double value) {newcolour.set_value<double>(name,value, false);})

  .def("keys", [](GMAD::NewColour &self) {return self.AllNames();})
  .def("__len__", [](GMAD::NewColour &self) {return self.AllNames().size();})
  .def("__setitem__", [](GMAD::NewColour &self, const std::string& key, int value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::NewColour &self, const std::string& key, double value) {self.set_value(key,value, false);})
  .def("__setitem__", [](GMAD::NewColour &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
  .def("_ipython_key_completions_", [](GMAD::NewColour &self) {return self.AllNames();});

}

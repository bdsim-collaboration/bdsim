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
// Created by Stewart Boogert on 11/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "atom.h"

PYBIND11_MODULE(atom, m) {
  py::class_<GMAD::Published<GMAD::Atom>>(m,"PublishedAtom")
    .def("NameExists",&GMAD::Atom::NameExists)
    .def("AllNames", &GMAD::Atom::AllNames);

  py::class_<GMAD::Atom, GMAD::Published<GMAD::Atom>>(m,"Atom")
    .def(py::init<>())
    .def("clear",&GMAD::Atom::clear)
    .def("print",&GMAD::Atom::print)

    .def_readonly("name", &GMAD::Atom::name)
    .def_readonly("A", &GMAD::Atom::A)
    .def_readonly("Z", &GMAD::Atom::Z)
    .def_readonly("symbol", &GMAD::Atom::symbol)

    .def("set_value",[](GMAD::Atom &self,std::string name,bool value) {self.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Atom &self,std::string name,int value) {self.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Atom &self,std::string name,long int value) {self.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Atom &self,std::string name,double value) {self.set_value<double>(name,value);})
    .def("set_value",[](GMAD::Atom &self,std::string name,std::string value) {self.set_value<std::string>(name,value);})
    .def("get_value",[](GMAD::Atom &self,std::string name) {
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

    .def("keys", [](GMAD::Atom &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Atom &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Atom &self, const std::string& key, int value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::Atom &self, const std::string& key, double value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::Atom &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Atom &self) {return self.AllNames();});
}
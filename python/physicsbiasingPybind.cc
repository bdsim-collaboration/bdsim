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

#include "physicsbiasing.h"
#include "array.h"

PYBIND11_MODULE(physicsbiasing, m) {
  py::class_<GMAD::Published<GMAD::PhysicsBiasing>>(m, "PublishedPhysicsBiasing")
    .def("NameExists", &GMAD::PhysicsBiasing::NameExists)
    .def("AllNames",&GMAD::PhysicsBiasing::AllNames);

  py::class_<GMAD::PhysicsBiasing, GMAD::Published<GMAD::PhysicsBiasing>>(m,"PhysicsBiasing")
    .def(py::init<>())

    .def("clear",&GMAD::PhysicsBiasing::clear)
    .def("print",&GMAD::PhysicsBiasing::print)

    .def_readonly("name",&GMAD::PhysicsBiasing::name)
    .def_readonly("particle",&GMAD::PhysicsBiasing::particle)
    .def_readonly("process",&GMAD::PhysicsBiasing::process)
    .def_readonly("processList",&GMAD::PhysicsBiasing::processList)
    .def_readonly("factor",&GMAD::PhysicsBiasing::factor)
    .def_readonly("flag",&GMAD::PhysicsBiasing::flag)

    .def("set_value",[](GMAD::PhysicsBiasing &self, const std::string& property, double value) {self.set_value(property,value,false);})
    .def("set_value",[](GMAD::PhysicsBiasing &self, const std::string& property, std::string value) {self.set_value(property,value,false);})
    .def("set_value",[](GMAD::PhysicsBiasing &self, const std::string& property, GMAD::Array* value) {self.set_value(property,value,false);})
    .def("get_value",[](GMAD::PhysicsBiasing &self,std::string name) {
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

      try {
        auto arrval = self.get_value_array(name);
        py::list result;
        for (const auto& value : arrval) {
          result.append(value);
        }
        retval = result;
        return retval;
      }
      catch (const std::runtime_error&) {}

      throw std::runtime_error("name not found : "+name);
    })

    .def("keys", [](GMAD::PhysicsBiasing &self) {return self.AllNames();})
    .def("__len__", [](GMAD::PhysicsBiasing &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::PhysicsBiasing &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::PhysicsBiasing &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::PhysicsBiasing &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::PhysicsBiasing &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::PhysicsBiasing &self, const std::string& key, py::list &value) {
      py::module_ m = py::module_::import("bdsim");
      py::object cls = m.attr("Array");  // get the class
      py::object obj = cls(value);       // call constructor
      auto array = obj.cast<GMAD::Array*>();
      self.set_value(key, array, false);
    })
    .def("_ipython_key_completions_", [](GMAD::PhysicsBiasing &self) {return self.AllNames();});
}
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

#include "samplerplacement.h"

PYBIND11_MODULE(samplerplacement, m) {
  py::class_<GMAD::Published<GMAD::SamplerPlacement>>(m,"PublishedSamplerPlacement")
    .def("NameExists", &GMAD::SamplerPlacement::NameExists)
    .def("AllNames", &GMAD::SamplerPlacement::AllNames);

  py::class_<GMAD::SamplerPlacement, GMAD::Published<GMAD::SamplerPlacement>>(m,"SamplerPlacement")
    .def (py::init<>())

    .def("clear",&GMAD::SamplerPlacement::clear)
    .def("print",&GMAD::SamplerPlacement::print)

    .def_readonly("name",&GMAD::SamplerPlacement::name)
    .def_readonly("samplerType",&GMAD::SamplerPlacement::samplerType)
    .def_readonly("referenceElement",&GMAD::SamplerPlacement::referenceElement)
    .def_readonly("referenceElementNumber",&GMAD::SamplerPlacement::referenceElementNumber)
    .def_readonly("s",&GMAD::SamplerPlacement::s)
    .def_readonly("x",&GMAD::SamplerPlacement::x)
    .def_readonly("y",&GMAD::SamplerPlacement::y)
    .def_readonly("z",&GMAD::SamplerPlacement::z)

    .def_readonly("phi",&GMAD::SamplerPlacement::phi)
    .def_readonly("theta",&GMAD::SamplerPlacement::theta)
    .def_readonly("psi",&GMAD::SamplerPlacement::psi)

    .def_readonly("axisX",&GMAD::SamplerPlacement::axisX)
    .def_readonly("axisY",&GMAD::SamplerPlacement::axisY)
    .def_readonly("axisZ",&GMAD::SamplerPlacement::axisZ)
    .def_readonly("angle",&GMAD::SamplerPlacement::angle)

    .def_readonly("axisAngle",&GMAD::SamplerPlacement::axisAngle)

    .def_readonly("apertureModel",&GMAD::SamplerPlacement::apertureModel)
    .def_readonly("shape",&GMAD::SamplerPlacement::shape)
    .def_readonly("aper1",&GMAD::SamplerPlacement::aper1)
    .def_readonly("aper2",&GMAD::SamplerPlacement::aper2)
    .def_readonly("aper3",&GMAD::SamplerPlacement::aper3)
    .def_readonly("aper4",&GMAD::SamplerPlacement::aper4)

    .def_readonly("startAnglePhi",&GMAD::SamplerPlacement::startAnglePhi)
    .def_readonly("sweepAnglePhi",&GMAD::SamplerPlacement::sweepAnglePhi)
    .def_readonly("startAngleTheta",&GMAD::SamplerPlacement::startAngleTheta)
    .def_readonly("sweepAngleTheta",&GMAD::SamplerPlacement::sweepAngleTheta)

    .def_readonly("partID",&GMAD::SamplerPlacement::partID)
    .def_readonly("partIDSetID",&GMAD::SamplerPlacement::partIDSetID)

    .def("set_value",[](GMAD::SamplerPlacement &self,std::string name,bool value) {self.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &self,std::string name,int value) {self.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &self,std::string name,long int value) {self.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &self,std::string name,double value) {self.set_value<double>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &self,std::string name,std::string value) {self.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &self,std::string name,GMAD::Array *value) {self.set_value(name,value,false);})
    .def("get_value",[](GMAD::SamplerPlacement &self,std::string name) {
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

    .def("keys", [](GMAD::SamplerPlacement &self) {return self.AllNames();})
    .def("__len__", [](GMAD::SamplerPlacement &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::SamplerPlacement &self, const std::string& key, bool value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::SamplerPlacement &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::SamplerPlacement &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::SamplerPlacement &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::SamplerPlacement &self, const std::string& key, py::list &value) {
      py::module_ mbdsim = py::module_::import("bdsim");
      py::object cls = mbdsim.attr("Array");  // get the class
      py::object obj = cls(value);       // call constructor
      auto array = obj.cast<GMAD::Array*>();
      self.set_value(key, array, false);
    })
    .def("_ipython_key_completions_", [](GMAD::SamplerPlacement &self) {return self.AllNames();});
}
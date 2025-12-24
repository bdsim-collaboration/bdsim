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
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "aperture.h"

/**
 * @author Stewart Boogert
 */


PYBIND11_MODULE(aperture, m)
{
  py::class_<GMAD::Published<GMAD::Aperture>>(m,"PublishedAperture")
    .def("NameExists",&GMAD::Aperture::NameExists)
    .def("AllNames", &GMAD::Aperture::AllNames);
  
  py::class_<GMAD::Aperture, GMAD::Published<GMAD::Aperture>>(m,"Aperture")
    .def(py::init<>())
    .def("clear",&GMAD::Aperture::clear)
    .def("print",&GMAD::Aperture::print)

    .def_readonly("name", &GMAD::Aperture::name)
    .def_readonly("apertureType", &GMAD::Aperture::apertureType)
    .def_readonly("aper1", &GMAD::Aperture::aper1)
    .def_readonly("aper2", &GMAD::Aperture::aper2)
    .def_readonly("aper3", &GMAD::Aperture::aper3)
    .def_readonly("aper4", &GMAD::Aperture::aper4)

    .def("set_value",[](GMAD::Aperture &self,std::string name,bool value) {self.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Aperture &self,std::string name,int value) {self.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Aperture &self,std::string name,long int value) {self.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Aperture &self,std::string name,double value) {self.set_value<double>(name,value);})
    .def("set_value",[](GMAD::Aperture &self,std::string name,std::string value) {self.set_value<std::string>(name,value);})
    .def("get_value",[](GMAD::Aperture &self,std::string name) {
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

    .def("keys", [](GMAD::Aperture &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Aperture &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Aperture &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Aperture &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Aperture &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    // __getitem__ is installed later in the init
    .def("_ipython_key_completions_", [](GMAD::Aperture &self) {return self.AllNames();});
}


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

#include "query.h"

PYBIND11_MODULE(query, m) {
  py::class_<GMAD::Published<GMAD::Query>>(m,"PublishedQuery")
    .def("NameExists", &GMAD::Query::NameExists)
    .def("AllNames", &GMAD::Query::AllNames);

  py::class_<GMAD::Query, GMAD::Published<GMAD::Query>>(m,"Query")
    .def (py::init<>())

    .def("clear",&GMAD::Query::clear)
    .def("print",&GMAD::Query::print)

    .def_readonly("name",&GMAD::Query::name)
    .def_readonly("nx",&GMAD::Query::nx)
    .def_readonly("ny",&GMAD::Query::ny)
    .def_readonly("nz",&GMAD::Query::nz)
    .def_readonly("nt",&GMAD::Query::nt)
    .def_readonly("xmin",&GMAD::Query::xmin)
    .def_readonly("xmax",&GMAD::Query::xmax)
    .def_readonly("ymin",&GMAD::Query::ymin)
    .def_readonly("ymax",&GMAD::Query::ymax)
    .def_readonly("zmin",&GMAD::Query::zmin)
    .def_readonly("zmax",&GMAD::Query::zmax)
    .def_readonly("tmin",&GMAD::Query::tmin)
    .def_readonly("tmax",&GMAD::Query::tmax)
    .def_readonly("outfileMagnetic",&GMAD::Query::outfileMagnetic)
    .def_readonly("outfileElectric",&GMAD::Query::outfileElectric)
    .def_readonly("fieldObject",&GMAD::Query::fieldObject)

    .def_readonly("queryMagneticField",&GMAD::Query::queryMagneticField)
    .def_readonly("queryElectricField",&GMAD::Query::queryElectricField)

    .def_readonly("overwriteExistingFiles",&GMAD::Query::overwriteExistingFiles)
    .def_readonly("printTransform",&GMAD::Query::printTransform)

    .def_readonly("drawArrows",&GMAD::Query::drawArrows)
    .def_readonly("drawZeroValuePoints",&GMAD::Query::drawZeroValuePoints)
    .def_readonly("drawBoxes",&GMAD::Query::drawBoxes)
    .def_readonly("boxAlpha",&GMAD::Query::boxAlpha)

    .def_readonly("pointsFile",&GMAD::Query::pointsFile)

    .def_readonly("referenceElement",&GMAD::Query::referenceElement)
    .def_readonly("referenceElementNumber",&GMAD::Query::referenceElementNumber)
    .def_readonly("s",&GMAD::Query::s)
    .def_readonly("x",&GMAD::Query::x)
    .def_readonly("y",&GMAD::Query::y)
    .def_readonly("z",&GMAD::Query::z)

    .def_readonly("phi",&GMAD::Query::phi)
    .def_readonly("theta",&GMAD::Query::theta)
    .def_readonly("psi",&GMAD::Query::psi)

    .def_readonly("axisX",&GMAD::Query::axisX)
    .def_readonly("axisY",&GMAD::Query::axisY)
    .def_readonly("axisZ",&GMAD::Query::axisZ)
    .def_readonly("angle",&GMAD::Query::angle)

    .def_readonly("axisAngle",&GMAD::Query::axisAngle)

    .def_readonly("checkParameters",&GMAD::Query::checkParameters)

    .def("set_value",[](GMAD::Query &self,std::string name,std::string value) {self.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Query &self,std::string name,int value) {self.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Query &self,std::string name,bool value) {self.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Query &self,std::string name,long int value) {self.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Query &self,std::string name,double value) {self.set_value<double>(name,value,false);})
    .def("get_value",[](GMAD::Query &self,std::string name) {
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

    .def("keys", [](GMAD::Query &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Query &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Query &self, const std::string& key, bool value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Query &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Query &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Query &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Query &self) {return self.AllNames();});

}
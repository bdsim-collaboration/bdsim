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

#include "region.h"

PYBIND11_MODULE(region, m) {
  py::class_<GMAD::Published<GMAD::Region>>(m,"PublishedRegion")
    .def("NameExists", &GMAD::Region::NameExists)
    .def("AllNames", &GMAD::Region::AllNames);

  py::class_<GMAD::Region, GMAD::Published<GMAD::Region>>(m,"Region")
    .def (py::init<>())

    .def("clear",&GMAD::Region::clear)
    .def("print",&GMAD::Region::print)

    .def_readonly("name",&GMAD::Region::name)
    .def_readonly("defaultRangeCut",&GMAD::Region::defaultRangeCut)
    .def_readonly("prodCutPhotons",&GMAD::Region::prodCutPhotons)
    .def_readonly("prodCutElectrons",&GMAD::Region::prodCutElectrons)
    .def_readonly("prodCutPositrons",&GMAD::Region::prodCutPositrons)
    .def_readonly("prodCutProtons",&GMAD::Region::prodCutProtons)

    .def("set_value",[](GMAD::Region &region,std::string name,std::string value) {region.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,int value) {region.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,bool value) {region.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,long int value) {region.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,double value) {region.set_value<double>(name,value,false);})

    .def("keys", [](GMAD::Region &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Region &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Region &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Region &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Region &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Region &self) {return self.AllNames();});

}
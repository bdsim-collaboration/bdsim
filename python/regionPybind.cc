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
    .def("NameExists",&GMAD::Region::NameExists);

  py::class_<GMAD::Region>(m,"Region")
    .def (py::init<>())
    .def_readwrite("name",&GMAD::Region::name)
    .def_readwrite("defaultRangeCut",&GMAD::Region::defaultRangeCut)
    .def_readwrite("prodCutPhotons",&GMAD::Region::prodCutPhotons)
    .def_readwrite("prodCutElectrons",&GMAD::Region::prodCutElectrons)
    .def_readwrite("prodCutPositrons",&GMAD::Region::prodCutPositrons)
    .def_readwrite("prodCutProtons",&GMAD::Region::prodCutProtons)

    .def("clear",&GMAD::Region::clear)
    .def("print",&GMAD::Region::print)

    .def("set_value",[](GMAD::Region &region,std::string name,std::string value) {region.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,int value) {region.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,bool value) {region.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,long int value) {region.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Region &region,std::string name,double value) {region.set_value<double>(name,value,false);});
}
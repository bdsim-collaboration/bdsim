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

#include "tunnel.h"

PYBIND11_MODULE(tunnel, m) {
  py::class_<GMAD::Published<GMAD::Tunnel>>(m,"PublishedTunnel")
    .def("NameExists", &GMAD::Tunnel::NameExists)
    .def("AllNames", &GMAD::Tunnel::AllNames);

  py::class_<GMAD::Tunnel, GMAD::Published<GMAD::Tunnel>>(m,"Tunnel")
    .def (py::init<>())

    .def("clear",&GMAD::Tunnel::clear)
    .def("print",&GMAD::Tunnel::print)

    .def_readonly("name",&GMAD::Tunnel::name)
    .def_readonly("type",&GMAD::Tunnel::type)

    .def_readonly("aper1",&GMAD::Tunnel::aper1)
    .def_readonly("aper2",&GMAD::Tunnel::aper2)

    .def_readonly("offsetX",&GMAD::Tunnel::offsetX)
    .def_readonly("offsetY",&GMAD::Tunnel::offsetY)

    .def_readonly("thickness",&GMAD::Tunnel::thickness)
    .def_readonly("soilThickness",&GMAD::Tunnel::soilThickness)
    .def_readonly("floorOffset",&GMAD::Tunnel::floorOffset)

    .def_readonly("visible",&GMAD::Tunnel::visible)

    .def("set_value",[](GMAD::Tunnel &self,std::string name,bool value) {self.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &self,std::string name,int value) {self.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &self,std::string name,long int value) {self.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &self,std::string name,double value) {self.set_value<double>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &self,std::string name,std::string value) {self.set_value<std::string>(name,value,false);})

    .def("keys", [](GMAD::Tunnel &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Tunnel &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Tunnel &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Tunnel &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Tunnel &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Tunnel &self) {return self.AllNames();});
}
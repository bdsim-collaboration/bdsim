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

#include "parameters.h"
PYBIND11_MAKE_OPAQUE(std::map<std::string, bool>)

PYBIND11_MODULE(parameters, m) {
  py::class_<GMAD::Parameters, GMAD::Element, GMAD::Published<GMAD::Element> >(m,"Parameters")
    .def(py::init<>())
    .def_readwrite("setMap",&GMAD::Parameters::setMap)
    .def("inherit_properties",&GMAD::Parameters::inherit_properties)

    .def("set_value",[](GMAD::Parameters &parameters,std::string name,std::string value) {parameters.set_value<std::string>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,int value) {parameters.set_value<int>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,bool value) {parameters.set_value<bool>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,long int value) {parameters.set_value<long int>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,double value) {parameters.set_value<double>(name,value, false);});
}
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
.def("NameExists",&GMAD::Atom::NameExists);

  py::class_<GMAD::Atom, GMAD::Published<GMAD::Atom>>(m,"Atom")
    .def(py::init<>())
    .def_readwrite("name", &GMAD::Atom::name)
    .def_readwrite("A", &GMAD::Atom::A)
    .def_readwrite("Z", &GMAD::Atom::Z)
    .def_readwrite("symbol", &GMAD::Atom::symbol)
    .def("clear",&GMAD::Atom::clear)
    .def("print",&GMAD::Atom::print)
    .def("set_value",[](GMAD::Atom &atom,std::string name,std::string value) {atom.set_value<std::string>(name,value);})
    .def("set_value",[](GMAD::Atom &atom,std::string name,int value) {atom.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Atom &atom,std::string name,bool value) {atom.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Atom &atom,std::string name,long int value) {atom.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Atom &atom,std::string name,double value) {atom.set_value<double>(name,value);});
}
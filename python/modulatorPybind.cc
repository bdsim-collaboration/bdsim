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

#include "modulator.h"

PYBIND11_MODULE(modulator, m) {
  py::class_<GMAD::Published<GMAD::Modulator>>(m,"PublishedModulator")
    .def("NameExists", &GMAD::Modulator::NameExists)
    .def("AllNames", &GMAD::Modulator::AllNames);

  py::class_<GMAD::Modulator, GMAD::Published<GMAD::Modulator>>(m,"Modulator")
    .def (py::init<>())
    .def("clear",&GMAD::Modulator::clear)
    .def("print",&GMAD::Modulator::print)

    .def_readonly("name",&GMAD::Modulator::name)
    .def_readonly("type",&GMAD::Modulator::type)
    .def_readonly("frequency",&GMAD::Modulator::frequency)
    .def_readonly("phase",&GMAD::Modulator::phase)
    .def_readonly("tOffset",&GMAD::Modulator::tOffset)
    .def_readonly("amplitudeOffset",&GMAD::Modulator::amplitudeOffset)
    .def_readonly("amplitudeScale",&GMAD::Modulator::amplitudeScale)

    .def_readonly("T0",&GMAD::Modulator::T0)
    .def_readonly("T1",&GMAD::Modulator::T1)

    .def("set_value",[](GMAD::Modulator &modulator,std::string name,std::string value) {modulator.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,int value) {modulator.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,bool value) {modulator.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,long int value) {modulator.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,double value) {modulator.set_value<double>(name,value,false);})

    .def("keys", [](GMAD::Modulator &self) {return self.AllNames();})
    .def("__len__", [](GMAD::Modulator &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Modulator &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Modulator &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::Modulator &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::Modulator &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::Modulator &self) {return self.AllNames();});
}
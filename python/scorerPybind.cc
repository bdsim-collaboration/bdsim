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

#include "scorer.h"

PYBIND11_MODULE(scorer, m) {
    py::class_<GMAD::Published<GMAD::Scorer>>(m,"PublishedScorer")
      .def("NameExists", &GMAD::Scorer::NameExists)
      .def("AllNames", &GMAD::Scorer::AllNames);

    py::class_<GMAD::Scorer, GMAD::Published<GMAD::Scorer>>(m,"Scorer")
      .def (py::init<>())

      .def("clear",&GMAD::Scorer::clear)
      .def("print",&GMAD::Scorer::print)

      .def_readonly("name",&GMAD::Scorer::name)
      .def_readonly("type",&GMAD::Scorer::type)
      .def_readonly("particleName",&GMAD::Scorer::particleName)
      .def_readonly("particlePDGID",&GMAD::Scorer::particlePDGID)
      .def_readonly("minimumKineticEnergy",&GMAD::Scorer::minimumKineticEnergy)
      .def_readonly("maximumKineticEnergy",&GMAD::Scorer::maximumKineticEnergy)
      .def_readonly("minimumTime",&GMAD::Scorer::minimumTime)
      .def_readonly("maximumTime",&GMAD::Scorer::maximumTime)
      .def_readonly("conversionFactorFile",&GMAD::Scorer::conversionFactorFile)
      .def_readonly("conversionFactorPath",&GMAD::Scorer::conversionFactorPath)
      .def_readonly("materialToInclude",&GMAD::Scorer::materialToInclude)
      .def_readonly("materialToExclude",&GMAD::Scorer::materialToExclude)
      .def_readonly("scoreWorldVolumeOnly",&GMAD::Scorer::scoreWorldVolumeOnly)
      .def_readonly("scorePrimariesOnly",&GMAD::Scorer::scorePrimariesOnly)

      .def("set_value",[](GMAD::Scorer &scorer,std::string name,std::string value) {scorer.set_value<std::string>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,int value) {scorer.set_value<int>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,bool value) {scorer.set_value<bool>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,long int value) {scorer.set_value<long int>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,double value) {scorer.set_value<double>(name,value,false);})

      .def("keys", [](GMAD::Scorer &self) {return self.AllNames();})
      .def("__len__", [](GMAD::Scorer &self) {return self.AllNames().size();})
      .def("__setitem__", [](GMAD::Scorer &self, const std::string& key, int value) {self.set_value(key,value, false);})
      .def("__setitem__", [](GMAD::Scorer &self, const std::string& key, double value) {self.set_value(key,value, false);})
      .def("__setitem__", [](GMAD::Scorer &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
      .def("_ipython_key_completions_", [](GMAD::Scorer &self) {return self.AllNames();});
}
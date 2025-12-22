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
// Created by Stewart Boogert on 16/03/2023.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "coolingchannel.h"

PYBIND11_MODULE(coolingchannel, m) {
  py::class_<GMAD::Published<GMAD::CoolingChannel>>(m,"PublishedCoolingChannel")
    .def("NameExists",&GMAD::CoolingChannel::NameExists)
    .def("AllNames", &GMAD::CoolingChannel::AllNames);

  py::class_<GMAD::CoolingChannel, GMAD::Published<GMAD::CoolingChannel>>(m,"CoolingChannel")
    .def(py::init<>())
    .def("clear",&GMAD::CoolingChannel::clear)
    .def("print", &GMAD::CoolingChannel::print)

    .def_readwrite("name", &GMAD::CoolingChannel::name)
    .def_readonly("nCells", &GMAD::CoolingChannel::nCells)
    .def_readonly("cellLengthZ", &GMAD::CoolingChannel::cellLengthZ)
    .def_readonly("nCoils", &GMAD::CoolingChannel::nCoils)
    .def_readonly("coilInnerRadius", &GMAD::CoolingChannel::coilInnerRadius)
    .def_readonly("coilRadialThickness", &GMAD::CoolingChannel::coilRadialThickness)
    .def_readonly("coilLengthZ", &GMAD::CoolingChannel::coilLengthZ)
    .def_readonly("coilCurrent",&GMAD::CoolingChannel::coilCurrent)
    .def_readonly("coilOffsetZ",&GMAD::CoolingChannel::coilOffsetZ)
    .def_readonly("coilMaterial",&GMAD::CoolingChannel::coilMaterial)
    .def_readonly("mirrorCoils",&GMAD::CoolingChannel::mirrorCoils)
    .def_readonly("onAxisTolerance",&GMAD::CoolingChannel::onAxisTolerance)
    .def_readonly("nSheets",&GMAD::CoolingChannel::nSheets)

    .def_readonly("nDipoles",&GMAD::CoolingChannel::nDipoles)
    .def_readonly("dipoleAperture",&GMAD::CoolingChannel::dipoleAperture)
    .def_readonly("dipoleLengthZ",&GMAD::CoolingChannel::dipoleLengthZ)
    .def_readonly("dipoleFieldStrength",&GMAD::CoolingChannel::dipoleFieldStrength)
    .def_readonly("dipoleEngeCoefficient",&GMAD::CoolingChannel::dipoleEngeCoefficient)
    .def_readonly("dipoleOffsetZ",&GMAD::CoolingChannel::dipoleOffsetZ)

    .def_readonly("nAbsorbers",&GMAD::CoolingChannel::nAbsorbers)
    .def_readonly("absorberType",&GMAD::CoolingChannel::absorberType)
    .def_readonly("absorberMaterial",&GMAD::CoolingChannel::absorberMaterial)
    .def_readonly("absorberOffsetZ",&GMAD::CoolingChannel::absorberOffsetZ)
    .def_readonly("absorberCylinderLength",&GMAD::CoolingChannel::absorberCylinderLength)
    .def_readonly("absorberCylinderRadius",&GMAD::CoolingChannel::absorberCylinderRadius)
    .def_readonly("absorberWedgeOpeningAngle",&GMAD::CoolingChannel::absorberWedgeOpeningAngle)
    .def_readonly("absorberWedgeHeight",&GMAD::CoolingChannel::absorberWedgeHeight)
    .def_readonly("absorberWedgeRotationAngle",&GMAD::CoolingChannel::absorberWedgeRotationAngle)
    .def_readonly("absorberWedgeOffsetX",&GMAD::CoolingChannel::absorberWedgeOffsetX)
    .def_readonly("absorberWedgeOffsetY",&GMAD::CoolingChannel::absorberWedgeOffsetY)
    .def_readonly("absorberWedgeApexToBase",&GMAD::CoolingChannel::absorberWedgeApexToBase)

    .def_readonly("nRFCavities",&GMAD::CoolingChannel::nRFCavities)
    .def_readonly("rfOffsetZ",&GMAD::CoolingChannel::rfOffsetZ)
    .def_readonly("rfLength",&GMAD::CoolingChannel::rfLength)
    .def_readonly("rfVoltage",&GMAD::CoolingChannel::rfVoltage)
    .def_readonly("rfPhase",&GMAD::CoolingChannel::rfPhase)
    .def_readonly("rfFrequency",&GMAD::CoolingChannel::rfFrequency)
    .def_readonly("rfWindowThickness",&GMAD::CoolingChannel::rfWindowThickness)
    .def_readonly("rfWindowMaterial",&GMAD::CoolingChannel::rfWindowMaterial)
    .def_readonly("rfWindowRadius",&GMAD::CoolingChannel::rfWindowRadius)
    .def_readonly("rfCavityMaterial",&GMAD::CoolingChannel::rfCavityMaterial)
    .def_readonly("rfCavityVacuumMaterial",&GMAD::CoolingChannel::rfCavityVacuumMaterial)
    .def_readonly("rfCavityRadius",&GMAD::CoolingChannel::rfCavityRadius)
    .def_readonly("rfCavityThickness",&GMAD::CoolingChannel::rfCavityThickness)
    .def_readonly("rfTimeOffset",&GMAD::CoolingChannel::rfTimeOffset)

    .def_readonly("integrator",&GMAD::CoolingChannel::integrator)
    .def_readonly("magneticFieldModel",&GMAD::CoolingChannel::magneticFieldModel)
    .def_readonly("electricFieldModel",&GMAD::CoolingChannel::electricFieldModel)
    .def_readonly("dipoleFieldModel",&GMAD::CoolingChannel::dipoleFieldModel)

    .def("set_value",[](GMAD::CoolingChannel &cc,std::string name,std::string value) {cc.set_value<std::string>(name,value);})
    .def("set_value",[](GMAD::CoolingChannel &cc,std::string name,int value) {cc.set_value<int>(name,value);})
    .def("set_value",[](GMAD::CoolingChannel &cc,std::string name,long int value) {cc.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::CoolingChannel &cc,std::string name,double value) {cc.set_value<double>(name,value);})
    .def("set_value",[](GMAD::CoolingChannel &cc,std::string name,GMAD::Array *value) {cc.set_value(name,value);})

    .def("keys", [](GMAD::CoolingChannel &self) {return self.AllNames();})
    .def("__len__", [](GMAD::CoolingChannel &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::CoolingChannel &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::CoolingChannel &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::CoolingChannel &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("__setitem__", [](GMAD::CoolingChannel &self, const std::string& key, GMAD::Array *value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::CoolingChannel &self) {return self.AllNames();});

}
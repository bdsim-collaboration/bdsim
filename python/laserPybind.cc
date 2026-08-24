/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.
*/

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "laser.h"

namespace py = pybind11;

PYBIND11_MODULE(laser, m)
{
  py::class_<GMAD::Published<GMAD::Laser>>(m, "PublishedLaser")
    .def("NameExists", &GMAD::Laser::NameExists)
    .def("AllNames", &GMAD::Laser::AllNames);

  py::class_<GMAD::Laser, GMAD::Published<GMAD::Laser>>(m, "Laser")
    .def(py::init<>())
    .def("clear", &GMAD::Laser::clear)
    .def("print", &GMAD::Laser::print)
    .def_readonly("name", &GMAD::Laser::name)
    .def_readonly("wavelength", &GMAD::Laser::wavelength)
    .def_readonly("m2", &GMAD::Laser::m2)
    .def_readonly("pulseEnergy", &GMAD::Laser::pulseEnergy)
    .def_readonly("pulseDuration", &GMAD::Laser::pulseDuration)
    .def_readonly("w0", &GMAD::Laser::w0)
    .def_readonly("sigma0", &GMAD::Laser::sigma0)
    .def_readonly("laserArrivalTime", &GMAD::Laser::laserArrivalTime)
    .def_readonly("laserPolarization1", &GMAD::Laser::laserPolarization1)
    .def_readonly("laserPolarization2", &GMAD::Laser::laserPolarization2)
    .def_readonly("laserPolarization3", &GMAD::Laser::laserPolarization3)
    .def_readonly("ignoreRayleighRange", &GMAD::Laser::ignoreRayleighRange)
    .def("set_value", [](GMAD::Laser& self, const std::string& name, bool value)
      {self.set_value<bool>(name, value);})
    .def("set_value", [](GMAD::Laser& self, const std::string& name, double value)
      {self.set_value<double>(name, value);})
    .def("set_value", [](GMAD::Laser& self, const std::string& name, const std::string& value)
      {self.set_value<std::string>(name, value);})
    .def("get_value", [](GMAD::Laser& self, const std::string& name) -> py::object
      {
        try {return py::cast(self.get<bool>(&self, name));}
        catch (const std::runtime_error&) {}
        try {return py::cast(self.get<double>(&self, name));}
        catch (const std::runtime_error&) {}
        try {return py::cast(self.get<std::string>(&self, name));}
        catch (const std::runtime_error&) {}
        throw std::runtime_error("name not found : " + name);
      })
    .def("keys", [](GMAD::Laser& self) {return self.AllNames();})
    .def("__len__", [](GMAD::Laser& self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::Laser& self, const std::string& key, bool value)
      {self.set_value(key, value);})
    .def("__setitem__", [](GMAD::Laser& self, const std::string& key, double value)
      {self.set_value(key, value);})
    .def("__setitem__", [](GMAD::Laser& self, const std::string& key, const std::string& value)
      {self.set_value(key, value);})
    .def("_ipython_key_completions_", [](GMAD::Laser& self) {return self.AllNames();});
}

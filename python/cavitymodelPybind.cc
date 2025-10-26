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

#include "cavitymodel.h"

PYBIND11_MODULE(cavitymodel, m) {
  py::class_<GMAD::Published<GMAD::CavityModel>>(m,"PublishedCavityModel")
  .def("NameExists",&GMAD::CavityModel::NameExists);

  py::class_<GMAD::CavityModel, GMAD::Published<GMAD::CavityModel>>(m,"CavityModel")
  .def(py::init<>())
  .def_readwrite("name", &GMAD::CavityModel::name)
  .def_readwrite("type", &GMAD::CavityModel::type)
  .def_readwrite("material", &GMAD::CavityModel::material)
  .def_readwrite("irisRadius", &GMAD::CavityModel::irisRadius)
  .def_readwrite("equatorRadius", &GMAD::CavityModel::equatorRadius)
  .def_readwrite("halfCellLength", &GMAD::CavityModel::halfCellLength)
  .def_readwrite("equatorHorizontalAxis", &GMAD::CavityModel::equatorHorizontalAxis)
  .def_readwrite("equatorVerticalAxis", &GMAD::CavityModel::equatorVerticalAxis)
  .def_readwrite("irisHorizontalAxis",&GMAD::CavityModel::irisHorizontalAxis)
  .def_readwrite("irisVerticalAxis", &GMAD::CavityModel::irisVerticalAxis)
  .def_readwrite("tangetLineAngle", &GMAD::CavityModel::tangentLineAngle)
  .def_readwrite("thickness", &GMAD::CavityModel::thickness)
  .def_readwrite("numberOfPoints", &GMAD::CavityModel::numberOfPoints)
  .def_readwrite("numberOfCells", &GMAD::CavityModel::numberOfCells)
  .def("clear",&GMAD::CavityModel::clear)
  .def("PublishMembers", &GMAD::CavityModel::PublishMembers)
  .def("print", &GMAD::CavityModel::print)

  .def("set_value",[](GMAD::CavityModel &cm,std::string name,std::string value) {cm.set_value<std::string>(name,value);})
  .def("set_value",[](GMAD::CavityModel &cm,std::string name,int value) {cm.set_value<int>(name,value);})
  .def("set_value",[](GMAD::CavityModel &cm,std::string name,long int value) {cm.set_value<long int>(name,value);})
  .def("set_value",[](GMAD::CavityModel &cm,std::string name,double value) {cm.set_value<double>(name,value);});


}
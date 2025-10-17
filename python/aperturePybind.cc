/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2023.

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
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "aperture.h"

/**
 * @author Stewart Boogert
 */


PYBIND11_MODULE(aperture, m)
{
  py::class_<GMAD::Published<GMAD::Aperture>>(m,"PublishedAperture")
    .def("NameExists",&GMAD::Aperture::NameExists);
  
  py::class_<GMAD::Aperture, GMAD::Published<GMAD::Aperture>>(m,"Aperture")
    .def(py::init<>())
    .def_readwrite("name", &GMAD::Aperture::name)
    .def_readwrite("apertureType", &GMAD::Aperture::apertureType)
    .def_readwrite("aper1", &GMAD::Aperture::aper1)
    .def_readwrite("aper2", &GMAD::Aperture::aper2)
    .def_readwrite("aper3", &GMAD::Aperture::aper3)
    .def_readwrite("aper4", &GMAD::Aperture::aper4)
    .def("clear",&GMAD::Aperture::clear)
    .def("print",&GMAD::Aperture::print)
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,std::string value) {aperture.set_value<std::string>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,int value) {aperture.set_value<int>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,bool value) {aperture.set_value<bool>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,long int value) {aperture.set_value<long int>(name,value);})
    .def("set_value",[](GMAD::Aperture &aperture,std::string name,double value) {aperture.set_value<double>(name,value);});
}

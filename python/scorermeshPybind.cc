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
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "scorermesh.h"

PYBIND11_MODULE(scorermesh, m)
{
  py::class_<GMAD::Published<GMAD::ScorerMesh>>(m,"PublishedScorerMesh")
    .def("NameExists", &GMAD::ScorerMesh::NameExists)
    .def("AllNames", &GMAD::ScorerMesh::AllNames);

  py::class_<GMAD::ScorerMesh, GMAD::Published<GMAD::ScorerMesh>>(m,"ScorerMesh")
    .def (py::init<>())

    .def("clear", &GMAD::ScorerMesh::clear)
    .def("print", &GMAD::ScorerMesh::print)

    .def_readonly("name", &GMAD::ScorerMesh::name)
    .def_readonly("scoreQuantity", &GMAD::ScorerMesh::scoreQuantity)
    .def_readonly("geometryType", &GMAD::ScorerMesh::geometryType)

    .def_readonly("nx", &GMAD::ScorerMesh::nx)
    .def_readonly("ny", &GMAD::ScorerMesh::ny)
    .def_readonly("nz", &GMAD::ScorerMesh::nz)
    .def_readonly("nr", &GMAD::ScorerMesh::nr)
    .def_readonly("nphi", &GMAD::ScorerMesh::nphi)
    .def_readonly("ne", &GMAD::ScorerMesh::ne)
    .def_readonly("xsize", &GMAD::ScorerMesh::xsize)
    .def_readonly("ysize", &GMAD::ScorerMesh::ysize)
    .def_readonly("zsize", &GMAD::ScorerMesh::zsize)
    .def_readonly("rInner", &GMAD::ScorerMesh::rInner)
    .def_readonly("rOuter", &GMAD::ScorerMesh::rOuter)
    .def_readonly("eLow", &GMAD::ScorerMesh::eLow)
    .def_readonly("eHigh", &GMAD::ScorerMesh::eHigh)
    .def_readonly("eScale", &GMAD::ScorerMesh::eScale)
    .def_readonly("eBinsEdgesFilenamePath", &GMAD::ScorerMesh::eBinsEdgesFilenamePath)

    .def_readonly("sequence",&GMAD::ScorerMesh::sequence)
    .def_readonly("referenceElement",&GMAD::ScorerMesh::referenceElement)
    .def_readonly("referenceElementNumber",&GMAD::ScorerMesh::referenceElementNumber)
    .def_readonly("s", &GMAD::ScorerMesh::s)
    .def_readonly("x", &GMAD::ScorerMesh::x)
    .def_readonly("y", &GMAD::ScorerMesh::y)
    .def_readonly("z", &GMAD::ScorerMesh::z)

    .def_readonly("phi", &GMAD::ScorerMesh::phi)
    .def_readonly("theta", &GMAD::ScorerMesh::theta)
    .def_readonly("psi", &GMAD::ScorerMesh::psi)

    .def_readonly("axisX", &GMAD::ScorerMesh::axisX)
    .def_readonly("axisY", &GMAD::ScorerMesh::axisY)
    .def_readonly("axisZ", &GMAD::ScorerMesh::axisZ)
    .def_readonly("angle", &GMAD::ScorerMesh::angle)

    .def_readonly("axisAngle", &GMAD::ScorerMesh::axisAngle)

    .def("set_value",[](GMAD::ScorerMesh& self, std::string name, bool value) {self.set_value<bool>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& self, std::string name, int value) {self.set_value<int>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& self, std::string name, long int value) {self.set_value<long int>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& self, std::string name, double value) {self.set_value<double>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& self, std::string name, std::string value) {self.set_value<std::string>(name, value, true);})

    .def("keys", [](GMAD::ScorerMesh &self) {return self.AllNames();})
    .def("__len__", [](GMAD::ScorerMesh &self) {return self.AllNames().size();})
    .def("__setitem__", [](GMAD::ScorerMesh &self, const std::string& key, int value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::ScorerMesh &self, const std::string& key, double value) {self.set_value(key,value, false);})
    .def("__setitem__", [](GMAD::ScorerMesh &self, const std::string& key, const std::string& value) {self.set_value(key, value, false);})
    .def("_ipython_key_completions_", [](GMAD::ScorerMesh &self) {return self.AllNames();});
}

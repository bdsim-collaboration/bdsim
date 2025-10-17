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

#include "scorermesh.h"

PYBIND11_MODULE(scorermesh, m)
{
  py::class_<GMAD::Published<GMAD::ScorerMesh>>(m,"PublishedScorerMesh")
    .def("NameExists",&GMAD::ScorerMesh::NameExists);

  py::class_<GMAD::ScorerMesh>(m,"ScorerMesh")
    .def (py::init<>())
    .def_readwrite("name", &GMAD::ScorerMesh::name)
    .def_readwrite("scoreQuantity", &GMAD::ScorerMesh::scoreQuantity)
    .def_readwrite("geometryType", &GMAD::ScorerMesh::geometryType)

    .def_readwrite("nx", &GMAD::ScorerMesh::nx)
    .def_readwrite("ny", &GMAD::ScorerMesh::ny)
    .def_readwrite("nz", &GMAD::ScorerMesh::nz)
    .def_readwrite("nr", &GMAD::ScorerMesh::nr)
    .def_readwrite("nphi", &GMAD::ScorerMesh::nphi)
    .def_readwrite("ne", &GMAD::ScorerMesh::ne)
    .def_readwrite("xsize", &GMAD::ScorerMesh::xsize)
    .def_readwrite("ysize", &GMAD::ScorerMesh::ysize)
    .def_readwrite("zsize", &GMAD::ScorerMesh::zsize)
    .def_readwrite("rsize", &GMAD::ScorerMesh::rsize)
    .def_readwrite("eLow", &GMAD::ScorerMesh::eLow)
    .def_readwrite("eHigh", &GMAD::ScorerMesh::eHigh)
    .def_readwrite("eScale", &GMAD::ScorerMesh::eScale)
    .def_readwrite("eBinsEdgesFilenamePath", &GMAD::ScorerMesh::eBinsEdgesFilenamePath)

    .def_readwrite("sequence",&GMAD::ScorerMesh::sequence)
    .def_readwrite("referenceElement",&GMAD::ScorerMesh::referenceElement)
    .def_readwrite("referenceElementNumber",&GMAD::ScorerMesh::referenceElementNumber)
    .def_readwrite("s", &GMAD::ScorerMesh::s)
    .def_readwrite("x", &GMAD::ScorerMesh::x)
    .def_readwrite("y", &GMAD::ScorerMesh::y)
    .def_readwrite("z", &GMAD::ScorerMesh::z)

    .def_readwrite("phi", &GMAD::ScorerMesh::phi)
    .def_readwrite("theta", &GMAD::ScorerMesh::theta)
    .def_readwrite("psi", &GMAD::ScorerMesh::psi)

    .def_readwrite("axisX", &GMAD::ScorerMesh::axisX)
    .def_readwrite("axisY", &GMAD::ScorerMesh::axisY)
    .def_readwrite("axisZ", &GMAD::ScorerMesh::axisZ)
    .def_readwrite("angle", &GMAD::ScorerMesh::angle)

    .def_readwrite("axisAngle", &GMAD::ScorerMesh::axisAngle)

    .def("clear", &GMAD::ScorerMesh::clear)
    .def("print", &GMAD::ScorerMesh::print)

    .def("set_value",[](GMAD::ScorerMesh& scorermesh, std::string name, std::string value) {scorermesh.set_value<std::string>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& scorermesh, std::string name, int value) {scorermesh.set_value<int>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& scorermesh, std::string name, bool value) {scorermesh.set_value<bool>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& scorermesh, std::string name, long int value) {scorermesh.set_value<long int>(name, value, true);})
    .def("set_value",[](GMAD::ScorerMesh& scorermesh, std::string name, double value) {scorermesh.set_value<double>(name, value, true);});
}

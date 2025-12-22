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

#include "array.h"
#include "sym_table.h"

PYBIND11_MODULE(array, m) {
  py::class_<GMAD::Array>(m,"Array")
    .def(py::init<>())
    .def(py::init<GMAD::Symtab*>())
    // TODO static constructors
    .def("GetSymbols", &GMAD::Array::GetSymbols)
    .def("GetSymbolsList", &GMAD::Array::GetSymbolsList)
    .def("GetData", &GMAD::Array::GetData)
    .def("GetDataList", &GMAD::Array::GetDataList)
    .def("Clear", &GMAD::Array::Clear)
    .def("Print", &GMAD::Array::Print)
    .def("Copy", [](GMAD::Array &self, std::list<std::string> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::list<int> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::list<long int> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::list<float> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::list<double> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::vector<std::string> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::vector<int> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::vector<long int> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::vector<float> value) {self.Copy(value);})
    .def("Copy", [](GMAD::Array &self, std::vector<double> value) {self.Copy(value);});


    // TODO copy of STL into array
    // TODO copy of array into STL
}
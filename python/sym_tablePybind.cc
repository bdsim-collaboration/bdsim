//
// Created by Stewart Boogert on 11/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "sym_table.h"
#include "array.h"

PYBIND11_MODULE(sym_table, m) {
  py::class_<GMAD::Symtab>(m,"Symtab")
    .def(py::init<std::string>())
    .def("Set",[](GMAD::Symtab symtab, GMAD::Array* a ) {symtab.Set(a); })
    .def("Set",[](GMAD::Symtab symtab, std::string str ) {symtab.Set(str); })
    .def("Set",[](GMAD::Symtab symtab, double val, bool reserved=false ) {symtab.Set(val, reserved); })
    //.def("Set",[](GMAD::Symtab symtab, double (*function)(double) fn ) {symtab.Set(fn); });
    .def("GetName",&GMAD::Symtab::GetName)
    .def("GetType",&GMAD::Symtab::GetType)
    .def("IsReserved",&GMAD::Symtab::IsReserved)
    .def("GetArray",&GMAD::Symtab::GetArray)
    .def("GetNumber",&GMAD::Symtab::GetNumber)
    //.def("GetFunction",[]() {});
    .def("Print",&GMAD::Symtab::Print);
}
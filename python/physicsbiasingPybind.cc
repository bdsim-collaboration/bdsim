//
// Created by Stewart Boogert on 18/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "physicsbiasing.h"
#include "array.h"

PYBIND11_MODULE(physicsbiasing, m) {
  py::class_<GMAD::PhysicsBiasing>(m,"PhysicsBiasing")
    .def(py::init<>())
    .def_readwrite("name",&GMAD::PhysicsBiasing::name)
    .def_readwrite("particle",&GMAD::PhysicsBiasing::particle)
    .def_readwrite("process",&GMAD::PhysicsBiasing::process)
    .def_readwrite("processList",&GMAD::PhysicsBiasing::processList)
    .def_readwrite("factor",&GMAD::PhysicsBiasing::factor)
    .def_readwrite("flag",&GMAD::PhysicsBiasing::flag)

    .def("clear",&GMAD::PhysicsBiasing::clear)
    .def("print",&GMAD::PhysicsBiasing::print)
    .def("set_value",[](GMAD::PhysicsBiasing &biasing, const std::string& property, double value) {biasing.set_value(property,value);})
    .def("set_value",[](GMAD::PhysicsBiasing &biasing, const std::string& property, GMAD::Array* value) {biasing.set_value(property,value);})
    .def("set_value",[](GMAD::PhysicsBiasing &biasing, const std::string& property, std::string value) {biasing.set_value(property,value);});
}
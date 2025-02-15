//
// Created by Stewart Boogert on 19/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "parameters.h"
PYBIND11_MAKE_OPAQUE(std::map<std::string, bool>);

PYBIND11_MODULE(parameters, m) {
  py::class_<GMAD::Parameters, GMAD::Element, GMAD::Published<GMAD::Element> >(m,"Parameters")
    .def(py::init<>())
    .def_readwrite("setMap",&GMAD::Parameters::setMap)
    .def("inherit_properties",&GMAD::Parameters::inherit_properties)

    .def("set_value",[](GMAD::Parameters &parameters,std::string name,std::string value) {parameters.set_value<std::string>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,int value) {parameters.set_value<int>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,bool value) {parameters.set_value<bool>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,long int value) {parameters.set_value<long int>(name,value, false);})
    .def("set_value",[](GMAD::Parameters &parameters,std::string name,double value) {parameters.set_value<double>(name,value, false);});
}
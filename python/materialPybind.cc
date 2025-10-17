//
// Created by Stewart Boogert on 16/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "material.h"

PYBIND11_MODULE(material, m) {
  py::class_<GMAD::Published<GMAD::Material>>(m,"PublishedMaterial")
  .def("NameExists",&GMAD::Material::NameExists);

  py::class_<GMAD::Material, GMAD::Published<GMAD::Material>>(m,"Material")
  .def(py::init<>())
  .def_readwrite("A",&GMAD::Material::A)
  .def_readwrite("Z",&GMAD::Material::Z)
  .def_readwrite("density",&GMAD::Material::density)
  .def_readwrite("temper",&GMAD::Material::temper)
  .def_readwrite("pressure",&GMAD::Material::pressure)
  .def_readwrite("state", &GMAD::Material::state)
  .def_readwrite("components", &GMAD::Material::components)
  .def_readwrite("componentsFractions", &GMAD::Material::componentsFractions)
  .def_readwrite("componentsWeights", &GMAD::Material::componentsWeights)
  .def("clear",&GMAD::Material::clear)
  .def("print",&GMAD::Material::print)
  .def("set_value",[](GMAD::Material &material,std::string name,std::string value) {material.set_value<std::string>(name,value, false);})
  .def("set_value",[](GMAD::Material &material,std::string name,int value) {material.set_value<int>(name,value, false);})
  .def("set_value",[](GMAD::Material &material,std::string name,bool value) {material.set_value<bool>(name,value, false);})
  .def("set_value",[](GMAD::Material &material,std::string name,long int value) {material.set_value<long int>(name,value, false);})
  .def("set_value",[](GMAD::Material &material,std::string name,double value) {material.set_value<double>(name,value, false);})
  .def("set_value",[](GMAD::Material &material,std::string name,GMAD::Array *value) {material.set_value(name,value, false);});
}
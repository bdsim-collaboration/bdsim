//
// Created by Stewart Boogert on 16/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "crystal.h"

PYBIND11_MODULE(crystal, m) {
  py::class_<GMAD::Published<GMAD::Crystal>>(m,"PublishedCrystal")
  .def("NameExists",&GMAD::Crystal::NameExists);

  py::class_<GMAD::Crystal, GMAD::Published<GMAD::Crystal>>(m,"Crystal")
  .def(py::init<>())
  .def_readwrite("name", &GMAD::Crystal::name)
  .def_readwrite("material", &GMAD::Crystal::material)
  .def_readwrite("data", &GMAD::Crystal::data)
  .def_readwrite("shape", &GMAD::Crystal::shape)
  .def_readwrite("lengthX", &GMAD::Crystal::lengthX)
  .def_readwrite("lengthY", &GMAD::Crystal::lengthY)
  .def_readwrite("lengthZ", &GMAD::Crystal::lengthZ)
  .def_readwrite("sizeA", &GMAD::Crystal::sizeA)
  .def_readwrite("sizeB", &GMAD::Crystal::sizeB)
  .def_readwrite("sizeC", &GMAD::Crystal::sizeC)
  .def_readwrite("alpha", &GMAD::Crystal::alpha)
  .def_readwrite("beta", &GMAD::Crystal::beta)
  .def_readwrite("gamma", &GMAD::Crystal::gamma)
  .def_readwrite("spaceGroup", &GMAD::Crystal::spaceGroup)
  .def_readwrite("bendingAngleYAxis", &GMAD::Crystal::bendingAngleYAxis)
  .def_readwrite("bendingAngleZAxis", &GMAD::Crystal::bendingAngleZAxis)
  .def_readwrite("miscutAngleY", &GMAD::Crystal::miscutAngleY)
  .def("clear",&GMAD::Crystal::clear)
  .def("print",&GMAD::Crystal::print)
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,std::string value) {crystal.set_value<std::string>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,int value) {crystal.set_value<int>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,bool value) {crystal.set_value<bool>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,long int value) {crystal.set_value<long int>(name,value);})
  .def("set_value",[](GMAD::Crystal &crystal,std::string name,double value) {crystal.set_value<double>(name,value);});

}
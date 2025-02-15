//
// Created by Stewart Boogert on 16/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "field.h"

PYBIND11_MODULE(field, m) {
  py::class_<GMAD::Published<GMAD::Field>>(m,"PublishedField")
  .def("NameExists",&GMAD::Field::NameExists);

  py::class_<GMAD::Field, GMAD::Published<GMAD::Field>>(m,"Field")
  .def(py::init<>())

  .def_readwrite("name", &GMAD::Field::name)
  .def_readwrite("type", &GMAD::Field::type)
  .def_readwrite("eScaling", &GMAD::Field::eScaling)
  .def_readwrite("bScaling", &GMAD::Field::bScaling)
  .def_readwrite("integrator", &GMAD::Field::integrator)
  .def_readwrite("globalTransform", &GMAD::Field::globalTransform)
  .def_readwrite("magneticFile", &GMAD::Field::magneticFile)
  .def_readwrite("magneticInterpolator", &GMAD::Field::magneticInterpolator)
  .def_readwrite("electricFile", &GMAD::Field::electricFile)
  .def_readwrite("electricInterpolator", &GMAD::Field::electricInterpolator)
  .def_readwrite("fieldModulator", &GMAD::Field::fieldModulator)

  .def_readwrite("x", &GMAD::Field::x)
  .def_readwrite("y", &GMAD::Field::y)
  .def_readwrite("z", &GMAD::Field::x)
  .def_readwrite("t", &GMAD::Field::t)
  .def_readwrite("phi", &GMAD::Field::phi)
  .def_readwrite("theta", &GMAD::Field::theta)
  .def_readwrite("psi", &GMAD::Field::psi)
  .def_readwrite("axisX", &GMAD::Field::axisX)
  .def_readwrite("axisY", &GMAD::Field::axisY)
  .def_readwrite("axisZ", &GMAD::Field::axisZ)
  .def_readwrite("angle", &GMAD::Field::angle)
  .def_readwrite("axisAngle", &GMAD::Field::axisAngle)
  .def_readwrite("autoScale", &GMAD::Field::autoScale)
  .def_readwrite("maximumStepLength", &GMAD::Field::maximumStepLength)
  .def_readwrite("magneticSubField", &GMAD::Field::magneticSubField)
  .def_readwrite("electricSubField", &GMAD::Field::electricSubField)
  .def_readwrite("magneticReflection", &GMAD::Field::magneticReflection)
  .def_readwrite("electricReflection", &GMAD::Field::electricReflection)
  .def_readwrite("fieldParameters", &GMAD::Field::fieldParameters)
  .def("clear",&GMAD::Field::clear)
  .def("print",&GMAD::Field::print)
  .def("set_value",[](GMAD::Field &field,std::string name,std::string value) {field.set_value<std::string>(name,value, false);})
  .def("set_value",[](GMAD::Field &field,std::string name,int value) {field.set_value<int>(name,value, false);})
  .def("set_value",[](GMAD::Field &field,std::string name,bool value) {field.set_value<bool>(name,value, false);})
  .def("set_value",[](GMAD::Field &field,std::string name,long int value) {field.set_value<long int>(name,value, false);})
  .def("set_value",[](GMAD::Field &field,std::string name,double value) {field.set_value<double>(name,value, false);});
}

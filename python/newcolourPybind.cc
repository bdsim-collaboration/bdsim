//
// Created by Stewart Boogert on 17/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "newcolour.h"

PYBIND11_MODULE(newcolour, m) {
  py::class_<GMAD::Published<GMAD::NewColour>>(m,"PublishedNewColour")
  .def("NameExists",&GMAD::NewColour::NameExists);

  py::class_<GMAD::NewColour, GMAD::Published<GMAD::NewColour>>(m,"NewColour")
  .def(py::init<>())
  .def_readwrite("name",&GMAD::NewColour::name)
  .def_readwrite("red",&GMAD::NewColour::red)
  .def_readwrite("green",&GMAD::NewColour::green)
  .def_readwrite("blue",&GMAD::NewColour::blue)
  .def_readwrite("alpha",&GMAD::NewColour::alpha)
  .def("clear",&GMAD::NewColour::clear)
  .def("print",&GMAD::NewColour::print)
  .def("set_value",[](GMAD::NewColour &newcolour,std::string name,std::string value) {newcolour.set_value<std::string>(name,value, false);})
  .def("set_value",[](GMAD::NewColour &newcolour,std::string name,double value) {newcolour.set_value<double>(name,value, false);});
}

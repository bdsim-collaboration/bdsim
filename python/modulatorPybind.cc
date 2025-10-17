//
// Created by Stewart Boogert on 19/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "modulator.h"

PYBIND11_MODULE(modulator, m) {
  py::class_<GMAD::Published<GMAD::Modulator>>(m,"PublishedModulator")
    .def("NameExists",&GMAD::Modulator::NameExists);

  py::class_<GMAD::Modulator>(m,"Modulator")
    .def (py::init<>())

    .def_readwrite("name",&GMAD::Modulator::name)
    .def_readwrite("type",&GMAD::Modulator::type)
    .def_readwrite("frequency",&GMAD::Modulator::frequency)
    .def_readwrite("phase",&GMAD::Modulator::phase)
    .def_readwrite("tOffset",&GMAD::Modulator::tOffset)
    .def_readwrite("amplitudeOffset",&GMAD::Modulator::amplitudeOffset)
    .def_readwrite("amplitudeScale",&GMAD::Modulator::amplitudeScale)

    .def_readwrite("T0",&GMAD::Modulator::T0)
    .def_readwrite("T1",&GMAD::Modulator::T1)

    .def("clear",&GMAD::Modulator::clear)
    .def("print",&GMAD::Modulator::print)

    .def("set_value",[](GMAD::Modulator &modulator,std::string name,std::string value) {modulator.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,int value) {modulator.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,bool value) {modulator.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,long int value) {modulator.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Modulator &modulator,std::string name,double value) {modulator.set_value<double>(name,value,false);});
}
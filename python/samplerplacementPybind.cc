//
// Created by Stewart Boogert on 19/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "samplerplacement.h"

PYBIND11_MODULE(samplerplacement, m) {
  py::class_<GMAD::Published<GMAD::SamplerPlacement>>(m,"PublishedSamplerPlacement")
    .def("NameExists",&GMAD::SamplerPlacement::NameExists);

  py::class_<GMAD::SamplerPlacement>(m,"SamplerPlacement")
    .def (py::init<>())

    .def_readwrite("name",&GMAD::SamplerPlacement::name)
    .def_readwrite("samplerType",&GMAD::SamplerPlacement::samplerType)
    .def_readwrite("referenceElement",&GMAD::SamplerPlacement::referenceElement)
    .def_readwrite("referenceElementNumber",&GMAD::SamplerPlacement::referenceElementNumber)
    .def_readwrite("s",&GMAD::SamplerPlacement::s)
    .def_readwrite("x",&GMAD::SamplerPlacement::x)
    .def_readwrite("y",&GMAD::SamplerPlacement::y)
    .def_readwrite("z",&GMAD::SamplerPlacement::z)

    .def_readwrite("phi",&GMAD::SamplerPlacement::phi)
    .def_readwrite("theta",&GMAD::SamplerPlacement::theta)
    .def_readwrite("psi",&GMAD::SamplerPlacement::psi)

    .def_readwrite("axisX",&GMAD::SamplerPlacement::axisX)
    .def_readwrite("axisY",&GMAD::SamplerPlacement::axisY)
    .def_readwrite("axisZ",&GMAD::SamplerPlacement::axisZ)
    .def_readwrite("angle",&GMAD::SamplerPlacement::angle)

    .def_readwrite("axisAngle",&GMAD::SamplerPlacement::axisAngle)

    .def_readwrite("apertureModel",&GMAD::SamplerPlacement::apertureModel)
    .def_readwrite("shape",&GMAD::SamplerPlacement::shape)
    .def_readwrite("aper1",&GMAD::SamplerPlacement::aper1)
    .def_readwrite("aper2",&GMAD::SamplerPlacement::aper2)
    .def_readwrite("aper3",&GMAD::SamplerPlacement::aper3)
    .def_readwrite("aper4",&GMAD::SamplerPlacement::aper4)

    .def_readwrite("startAnglePhi",&GMAD::SamplerPlacement::startAnglePhi)
    .def_readwrite("sweepAnglePhi",&GMAD::SamplerPlacement::sweepAnglePhi)
    .def_readwrite("startAngleTheta",&GMAD::SamplerPlacement::startAngleTheta)
    .def_readwrite("sweepAngleTheta",&GMAD::SamplerPlacement::sweepAngleTheta)

    .def_readwrite("partID",&GMAD::SamplerPlacement::partID)
    .def_readwrite("partIDSetID",&GMAD::SamplerPlacement::partIDSetID)

    .def("clear",&GMAD::SamplerPlacement::clear)
    .def("print",&GMAD::SamplerPlacement::print)

    .def("set_value",[](GMAD::SamplerPlacement &samplerplacement,std::string name,std::string value) {samplerplacement.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &samplerplacement,std::string name,int value) {samplerplacement.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &samplerplacement,std::string name,bool value) {samplerplacement.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &samplerplacement,std::string name,long int value) {samplerplacement.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::SamplerPlacement &samplerplacement,std::string name,double value) {samplerplacement.set_value<double>(name,value,false);});
}
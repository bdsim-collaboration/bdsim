//
// Created by Stewart Boogert on 12/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "blmplacement.h"

PYBIND11_MODULE(blmplacement, m) {
py::class_<GMAD::Published<GMAD::BLMPlacement>>(m,
"PublishedBLMPlacement")
.def("NameExists",&GMAD::BLMPlacement::NameExists);

py::class_<GMAD::BLMPlacement, GMAD::Published<GMAD::BLMPlacement> >(m,
"BLMPlacement")
.def (py::init<>())
.def_readwrite("name", &GMAD::BLMPlacement::name)
.def_readwrite("referenceElement", &GMAD::BLMPlacement::referenceElement)
.def_readwrite("referenceElementNumber", &GMAD::BLMPlacement::referenceElementNumber)

.def_readwrite("s", &GMAD::BLMPlacement::s)
.def_readwrite("x", &GMAD::BLMPlacement::x)
.def_readwrite("y", &GMAD::BLMPlacement::y)
.def_readwrite("z", &GMAD::BLMPlacement::z)


.def_readwrite("phi", &GMAD::BLMPlacement::phi)
.def_readwrite("theta", &GMAD::BLMPlacement::theta)
.def_readwrite("psi", &GMAD::BLMPlacement::psi)


.def_readwrite("axisX", &GMAD::BLMPlacement::axisX)
.def_readwrite("axisY", &GMAD::BLMPlacement::axisY)
.def_readwrite("axisZ", &GMAD::BLMPlacement::axisZ)
.def_readwrite("angle", &GMAD::BLMPlacement::angle)

.def_readwrite("axisAngle", &GMAD::BLMPlacement::axisAngle)
.def_readwrite("side", &GMAD::BLMPlacement::side)
.def_readwrite("sideOffset", &GMAD::BLMPlacement::sideOffset)

.def_readwrite("geometryFile", &GMAD::BLMPlacement::geometryFile)
.def_readwrite("geometryType", &GMAD::BLMPlacement::geometryType)
.def_readwrite("blmMaterial", &GMAD::BLMPlacement::blmMaterial)
.def_readwrite("blm1", &GMAD::BLMPlacement::blm1)
.def_readwrite("blm2", &GMAD::BLMPlacement::blm2)
.def_readwrite("blm3", &GMAD::BLMPlacement::blm3)
.def_readwrite("blm4", &GMAD::BLMPlacement::blm4)
.def_readwrite("scoreQuantity", &GMAD::BLMPlacement::scoreQuantity)
.def_readwrite("bias", &GMAD::BLMPlacement::bias)

.def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,std::string value) {blmp.set_value<std::string>(name,value);})
.def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,int value) {blmp.set_value<int>(name,value);})
.def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,bool value) {blmp.set_value<bool>(name,value);})
.def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,long int value) {blmp.set_value<long int>(name,value);})
.def("set_value",[](GMAD::BLMPlacement &blmp,std::string name,double value) {blmp.set_value<double>(name,value);});

}

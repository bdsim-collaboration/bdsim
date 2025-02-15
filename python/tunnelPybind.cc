//
// Created by Stewart Boogert on 19/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "tunnel.h"

PYBIND11_MODULE(tunnel, m) {
  py::class_<GMAD::Published<GMAD::Tunnel>>(m,"PublishedTunnel")
    .def("NameExists",&GMAD::Tunnel::NameExists);

  py::class_<GMAD::Tunnel>(m,"Tunnel")
    .def (py::init<>())
    .def_readwrite("name",&GMAD::Tunnel::name)

    .def_readwrite("type",&GMAD::Tunnel::type)

    .def_readwrite("aper1",&GMAD::Tunnel::aper1)
    .def_readwrite("aper2",&GMAD::Tunnel::aper2)

    .def_readwrite("offsetX",&GMAD::Tunnel::offsetX)
    .def_readwrite("offsetY",&GMAD::Tunnel::offsetY)

    .def_readwrite("thickness",&GMAD::Tunnel::thickness)
    .def_readwrite("soilThickness",&GMAD::Tunnel::soilThickness)
    .def_readwrite("floorOffset",&GMAD::Tunnel::floorOffset)

    .def_readwrite("visible",&GMAD::Tunnel::visible)

    .def("clear",&GMAD::Tunnel::clear)
    .def("print",&GMAD::Tunnel::print)

    .def("set_value",[](GMAD::Tunnel &tunnel,std::string name,std::string value) {tunnel.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &tunnel,std::string name,int value) {tunnel.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &tunnel,std::string name,bool value) {tunnel.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &tunnel,std::string name,long int value) {tunnel.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Tunnel &tunnel,std::string name,double value) {tunnel.set_value<double>(name,value,false);});
}
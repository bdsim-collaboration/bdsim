//
// Created by Stewart Boogert on 18/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "query.h"

PYBIND11_MODULE(query, m) {
  py::class_<GMAD::Published<GMAD::Query>>(m,"PublishedQuery")
    .def("NameExists",&GMAD::Query::NameExists);

  py::class_<GMAD::Query>(m,"Query")
    .def (py::init<>())
    .def_readwrite("name",&GMAD::Query::name)
    .def_readwrite("nx",&GMAD::Query::nx)
    .def_readwrite("ny",&GMAD::Query::ny)
    .def_readwrite("nz",&GMAD::Query::nz)
    .def_readwrite("nt",&GMAD::Query::nt)
    .def_readwrite("xmin",&GMAD::Query::xmin)
    .def_readwrite("xmax",&GMAD::Query::xmax)
    .def_readwrite("ymin",&GMAD::Query::ymin)
    .def_readwrite("ymax",&GMAD::Query::ymax)
    .def_readwrite("zmin",&GMAD::Query::zmin)
    .def_readwrite("zmax",&GMAD::Query::zmax)
    .def_readwrite("tmin",&GMAD::Query::tmin)
    .def_readwrite("tmax",&GMAD::Query::tmax)
    .def_readwrite("outfileMagnetic",&GMAD::Query::outfileMagnetic)
    .def_readwrite("outfileElectric",&GMAD::Query::outfileElectric)
    .def_readwrite("fieldObject",&GMAD::Query::fieldObject)

    .def_readwrite("queryMagneticField",&GMAD::Query::queryMagneticField)
    .def_readwrite("queryElectricField",&GMAD::Query::queryElectricField)

    .def_readwrite("overwriteExistingFiles",&GMAD::Query::overwriteExistingFiles)
    .def_readwrite("printTransform",&GMAD::Query::printTransform)

    .def_readwrite("drawArrows",&GMAD::Query::drawArrows)
    .def_readwrite("drawZeroValuePoints",&GMAD::Query::drawZeroValuePoints)
    .def_readwrite("drawBoxes",&GMAD::Query::drawBoxes)
    .def_readwrite("boxAlpha",&GMAD::Query::boxAlpha)

    .def_readwrite("pointsFile",&GMAD::Query::pointsFile)

    .def_readwrite("referenceElement",&GMAD::Query::referenceElement)
    .def_readwrite("referenceElementNumber",&GMAD::Query::referenceElementNumber)
    .def_readwrite("s",&GMAD::Query::s)
    .def_readwrite("x",&GMAD::Query::x)
    .def_readwrite("y",&GMAD::Query::y)
    .def_readwrite("z",&GMAD::Query::z)

    .def_readwrite("phi",&GMAD::Query::phi)
    .def_readwrite("theta",&GMAD::Query::theta)
    .def_readwrite("psi",&GMAD::Query::psi)

    .def_readwrite("axisX",&GMAD::Query::axisX)
    .def_readwrite("axisY",&GMAD::Query::axisY)
    .def_readwrite("axisZ",&GMAD::Query::axisZ)
    .def_readwrite("angle",&GMAD::Query::angle)

    .def_readwrite("axisAngle",&GMAD::Query::axisAngle)

    .def_readwrite("checkParameters",&GMAD::Query::checkParameters)

    .def("clear",&GMAD::Query::clear)
    .def("print",&GMAD::Query::print)

    .def("set_value",[](GMAD::Query &query,std::string name,std::string value) {query.set_value<std::string>(name,value,false);})
    .def("set_value",[](GMAD::Query &query,std::string name,int value) {query.set_value<int>(name,value,false);})
    .def("set_value",[](GMAD::Query &query,std::string name,bool value) {query.set_value<bool>(name,value,false);})
    .def("set_value",[](GMAD::Query &query,std::string name,long int value) {query.set_value<long int>(name,value,false);})
    .def("set_value",[](GMAD::Query &query,std::string name,double value) {query.set_value<double>(name,value,false);});
}
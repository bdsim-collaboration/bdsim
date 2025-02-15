//
// Created by Stewart Boogert on 19/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "scorer.h"

PYBIND11_MODULE(scorer, m) {
    py::class_<GMAD::Published<GMAD::Scorer>>(m,"PublishedScorer")
      .def("NameExists",&GMAD::Scorer::NameExists);

    py::class_<GMAD::Scorer>(m,"Scorer")
      .def (py::init<>())
      .def_readwrite("name",&GMAD::Scorer::name)
      .def_readwrite("type",&GMAD::Scorer::type)
      .def_readwrite("particleName",&GMAD::Scorer::particleName)
      .def_readwrite("particlePDGID",&GMAD::Scorer::particlePDGID)
      .def_readwrite("minimumKineticEnergy",&GMAD::Scorer::minimumKineticEnergy)
      .def_readwrite("maximumKineticEnergy",&GMAD::Scorer::maximumKineticEnergy)
      .def_readwrite("minimumTime",&GMAD::Scorer::minimumTime)
      .def_readwrite("maximumTime",&GMAD::Scorer::maximumTime)
      .def_readwrite("conversionFactorFile",&GMAD::Scorer::conversionFactorFile)
      .def_readwrite("conversionFactorPath",&GMAD::Scorer::conversionFactorPath)
      .def_readwrite("materialToInclude",&GMAD::Scorer::materialToInclude)
      .def_readwrite("materialToExclude",&GMAD::Scorer::materialToExclude)
      .def_readwrite("scoreWorldVolumeOnly",&GMAD::Scorer::scoreWorldVolumeOnly)
      .def_readwrite("scorePrimariesOnly",&GMAD::Scorer::scorePrimariesOnly)

      .def("clear",&GMAD::Scorer::clear)
      .def("print",&GMAD::Scorer::print)

      .def("set_value",[](GMAD::Scorer &scorer,std::string name,std::string value) {scorer.set_value<std::string>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,int value) {scorer.set_value<int>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,bool value) {scorer.set_value<bool>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,long int value) {scorer.set_value<long int>(name,value,false);})
      .def("set_value",[](GMAD::Scorer &scorer,std::string name,double value) {scorer.set_value<double>(name,value,false);});
}
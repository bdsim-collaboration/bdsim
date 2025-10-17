//
// Created by Stewart Boogert on 15/03/2023.
//


#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include "BDSParser.hh"

PYBIND11_MODULE(bdsparser, m) {
  py::class_<BDSParser>(m,"BDSParser")
    .def(py::init<>())
    .def_static("Instance",[](std::string fileName) {return BDSParser::Instance(fileName);})// ,py::return_value_policy::reference)
    .def_static("Instance",[]() {return BDSParser::Instance();})
    .def_static("IsInitialised",&BDSParser::IsInitialised)
    .def("GetOptions", [](BDSParser &bdsp) {return &bdsp.GetOptions();}, py::return_value_policy::reference)
    .def("GetBeam",[](BDSParser &bdsp) {return &bdsp.GetBeam();}, py::return_value_policy::reference)
    .def("GetBeamline", [](BDSParser &bdsp) {return &bdsp.GetBeamline();}, py::return_value_policy::reference)
    .def("GetSamplerFilterIDToSet",[](BDSParser &bdsp) {return bdsp.GetSamplerFilterIDToSet();}, py::return_value_policy::reference)
    .def("GetSequence",&BDSParser::GetSequence)
    .def("GetElement", [](BDSParser &bdsp, const std::string& name) {
      std::cout << name << std::endl;
      return bdsp.GetElement(name);})
    .def("PrintBeamline", &BDSParser::PrintBeamline)
    .def("PrintElements", &BDSParser::PrintElements)
    .def("PrintOptions", &BDSParser::PrintOptions);
}

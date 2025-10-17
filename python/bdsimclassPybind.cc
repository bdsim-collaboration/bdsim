//
// Created by Stewart Boogert on 15/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSIMClass.hh"
#include "BDSParser.hh"

PYBIND11_MODULE(bdsimclass, m) {
py::class_<BDSIM>(m,"BDSIM")
    .def(py::init<>())
    .def(py::init<BDSParser *>())
    .def(py::init<>(
        [](std::vector<std::string> args, bool usualPrintOut=true) {
            std::vector<char *> cstrs;
            cstrs.reserve(args.size());
            for (auto &s: args) cstrs.push_back(const_cast<char *>(s.c_str()));
            return new BDSIM(args.size(), cstrs.data(), usualPrintOut);
        }))
    .def("Initialised",&BDSIM::Initialised)
    .def("InitialisationResult",&BDSIM::InitialisationResult)
    .def("BeamOn",&BDSIM::BeamOn);
}
//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoords.hh"

PYBIND11_MODULE(bdsparticlecoordsfull, m) {
  py::class_<BDSParticleCoordsFull, BDSParticleCoords>(m,"BDSParticleCoordsFull")
    .def(py::init<>())
    .def(py::init<G4double,  G4double,  G4double,
                  G4double, G4double, G4double,
                  G4double,  G4double,
                  G4double, G4double>(),
                  py::arg("x"),
                  py::arg("y"),
                  py::arg("z"),
                  py::arg("xp"),
                  py::arg("yp"),
                  py::arg("zp"),
                  py::arg("t"),
                  py::arg("s"),
                  py::arg("totalEnergy"),
                  py::arg("weight"))
    .def(py::init<BDSParticleCoords &, G4double, G4double, G4double>(),
         py::arg("local"),
         py::arg("s"),
         py::arg("totalEnergy"),
         py::arg("weight"))
     .def("__str__", [](const BDSParticleCoordsFull &self)
     {
       std::ostringstream oss;
       oss << self;
       return oss.str();
     })
     .def_readonly("s", &BDSParticleCoordsFull::s)
     .def_readonly("totalEnergy", &BDSParticleCoordsFull::totalEnergy)
     .def_readonly("weight", &BDSParticleCoordsFull::weight);

}
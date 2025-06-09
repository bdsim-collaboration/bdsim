//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSParticleCoords.hh"

PYBIND11_MODULE(bdsparticlecoords, m) {
  py::class_<BDSParticleCoords>(m, "BDSParticleCoords")
    .def(py::init<>())
    .def(py::init<G4double, G4double, G4double,
                  G4double, G4double, G4double,
                  G4double>(),
         py::arg("x"),
         py::arg("y"),
         py::arg("z"),
         py::arg("xp"),
         py::arg("yp"),
         py::arg("zp"),
         py::arg("t"))
    .def(py::init<>([](py::list pos,
                       py::list mom,
                       G4double t) {
      return new BDSParticleCoords(G4ThreeVector(pos[0].cast<double>(), pos[1].cast<double>(), pos[2].cast<double>()),
                                   G4ThreeVector(mom[0].cast<double>(), mom[1].cast<double>(), mom[2].cast<double>()),
                                   t);
    }))
    .def("__str__", [](const BDSParticleCoords &self)
    {
      std::ostringstream oss;
      oss << self;
      return oss.str();
     })
    .def("Position", [](const BDSParticleCoords &pc) {
      auto pv = pc.Position();
      py::list retval;
      retval.append(pv[0]);
      retval.append(pv[1]);
      retval.append(pv[2]);
      return retval;
    })
    .def("Momentum", [](const BDSParticleCoords &pc) {
      auto mv = pc.Momentum();
      py::list retval;
      retval.append(mv[0]);
      retval.append(mv[1]);
      retval.append(mv[2]);
      return retval;
    });
}
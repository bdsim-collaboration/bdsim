//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

PYBIND11_MODULE(g4particletable, m) {
  py::class_<G4ParticleTable>(m, "G4ParticleTable")
    .def("GetParticleTable", &G4ParticleTable::GetParticleTable, py::return_value_policy::reference)
    .def("contains", [](G4ParticleTable *pt, G4ParticleDefinition *pd) { return pt->contains(pd);})
    .def("contains", [](G4ParticleTable *pt, std::string name) { return pt->contains(name);})
    .def("entries", &G4ParticleTable::entries)
    .def("size", &G4ParticleTable::size)
    .def("DumpTable", [](G4ParticleTable *pt, std::string particle) {pt->DumpTable(particle);},
         py::arg("particle") = "ALL")
    .def("GetIonTable", &G4ParticleTable::GetIonTable, py::return_value_policy::reference)
    .def("RemoveAllParticles", &G4ParticleTable::RemoveAllParticles)
    .def("DeleteAllParticles", &G4ParticleTable::DeleteAllParticles)
    .def("SelectParticle", [](G4ParticleTable *pt, std::string name)
      {
        pt->SelectParticle(name);
      })
    .def("GetSelectedParticle", &G4ParticleTable::GetSelectedParticle);

}
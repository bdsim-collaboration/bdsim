//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;


#include <string>
#include <vector>

#include "BDSIMLink.hh"
#include "BDSBunch.hh"
#include "BDSLinkComponent.hh"

PYBIND11_MODULE(bdsimlink, m) {
  py::class_<BDSIMLink>(m, "BDSIMLink")
      .def(py::init([](BDSBunch *bunch) {
        return new BDSIMLink(bunch);
      }))
      .def("Initialise",[](BDSIMLink *link,
                           py::list py_argv,
                           bool usualPrintOut,
                           double minimumKineticEnergy,
                           bool protonsAndIonsOnly)
                           {
          auto argc = py_argv.size();

          std::vector<std::string> arg_strings;
          for (auto item: py_argv) {
            arg_strings.push_back(item.cast<std::string>());
          }

          // Create a char** array
          std::vector<char *> argv;
          for (auto &s: arg_strings) {
            argv.push_back(const_cast<char *>(s.c_str()));
          }

          return link->Initialise(argc,
                                  argv.data(),
                                  usualPrintOut,
                                  minimumKineticEnergy,
                                  protonsAndIonsOnly);
          },
          py::arg("argv"),
          py::arg("usualPrintOut") = TRUE,
          py::arg("minimumKineticEnergy") = 0,
          py::arg("protonsAndIonsOnly") = true)
      .def(py::init([](py::list py_argv) {
        auto argc = py_argv.size();

        std::vector<std::string> arg_strings;
        for (auto item: py_argv) {
          arg_strings.push_back(item.cast<std::string>());
        }

        // Create a char** array
        std::vector<char *> argv;
        for (auto &s: arg_strings) {
          argv.push_back(const_cast<char *>(s.c_str()));
        }
        return new BDSIMLink(argc, argv.data());
      }))
      .def("Initialised",&BDSIMLink::Initialised)
      .def("InitialisationResult",&BDSIMLink::InitialisationResult)
      .def("BeamOn",&BDSIMLink::BeamOn)
      .def("SelectLinkElement",[](BDSIMLink *bl, std::string elementName, bool debug){
        bl->SelectLinkElement(elementName,debug);}, py::arg("elementName"), py::arg("debug") = false)
      .def("SelectLinkElement",[](BDSIMLink *bl, int index, bool debug){
        bl->SelectLinkElement(index,debug);}, py::arg("elementName"), py::arg("debug") = false)
      .def("AddLinkCollimatorJaw", [] (BDSIMLink *bl,
          const std::string collimatorName,
          const std::string& materialName,
          double length,
          double halfApertureLeft,
          double halfApertureRight,
          double rotation,
          double xOffset,
          double yOffset,
          double jawTiltLeft = 0.0,
          double jawTiltRight = 0.0,
          bool   buildLeftJaw  = true,
          bool   buildRightJaw = true,
          bool   isACrystal    = false,
          double crystalAngle  = 0,
          bool   sampleIn      = false) {
        return bl->AddLinkCollimatorJaw(collimatorName,
                                        materialName,
                                        length,
                                        halfApertureLeft,
                                        halfApertureRight,
                                        rotation,
                                        xOffset,
                                        yOffset,
                                        jawTiltLeft,
                                        jawTiltRight,
                                        buildLeftJaw,
                                        buildRightJaw,
                                        isACrystal,
                                        crystalAngle,
                                        sampleIn);
      },
           py::arg("collimatorName"),
           py::arg("materialName"),
           py::arg("length"),
           py::arg("halfApertureLeft"),
           py::arg("halfApertureRight"),
           py::arg("rotation"),
           py::arg("xOffset"),
           py::arg("yOffSet"),
           py::arg("jawTiltLeft") = 0,
           py::arg("jawTiltRight") = 0,
           py::arg("buildLeftJaw") = true,
           py::arg("buildRightJaw") = true,
           py::arg("isACrystal") = false,
           py::arg("crystalAngle") = 0,
           py::arg("sampleIn") = false
           )
      .def("SamplerHits", &BDSIMLink::SamplerHits)
      .def("ClearSamplerHits",&BDSIMLink::ClearSamplerHits)
      .def("GetCurrentMaximumSixTrackParticleID", &BDSIMLink::GetCurrentMaximumSixTrackParticleID)
      .def("SetCurrentMaximumExternalParticleID", &BDSIMLink::SetCurrentMaximumExternalParticleID)
      .def("GetLinkIndex",&BDSIMLink::GetLinkIndex)
      .def("GetLinkComponent", &BDSIMLink::GetLinkComponent)
      .def("GetChordLengthOfLinkElement", [](BDSIMLink *link, int beamLineIndex)
        {return link->GetChordLengthOfLinkElement(beamLineIndex);})
      .def("GetChordLengthOfLinkElement", [](BDSIMLink *link, std::string elementName)
        {return link->GetChordLengthOfLinkElement(elementName);})
      .def("GetArcLengthOfLinkElement",[](BDSIMLink *link, int beamLineIndex)
        {return link->GetArcLengthOfLinkElement(beamLineIndex);})
      .def("GetArcLengthOfLinkElement",[](BDSIMLink *link, std::string elementName)
        {return link->GetArcLengthOfLinkElement(elementName);})
      .def("SelectLinkElement",[](BDSIMLink *link, int index, bool debug)
        {link->SelectLinkElement(index,debug);})
      .def("SelectLinkElement",[](BDSIMLink *link, int elementName, bool debug)
        {link->SelectLinkElement(elementName,debug);});
}

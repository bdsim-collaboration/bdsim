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
        py::arg("py_argv"),
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
      .def(py::init([](BDSBunch *bunch) {
        return new BDSIMLink(bunch);
      }))
      .def("Initialised",&BDSIMLink::Initialised)
      .def("InitialisationResult",&BDSIMLink::InitialisationResult)
      .def("BeamOn",&BDSIMLink::BeamOn)
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

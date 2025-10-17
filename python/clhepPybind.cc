//
// Created by Stewart Boogert on 15/03/2023.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
namespace py = pybind11;

#include <string>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

PYBIND11_MODULE(clhep, m) {
  m.attr("GeV") = py::cast(CLHEP::GeV);

  m.attr("nanometer") = py::cast(CLHEP::nanometer);
  m.attr("nm") = py::cast(CLHEP::nanometer);
  m.attr("micrometer") = py::cast(CLHEP::micrometer);
  m.attr("um") = py::cast(CLHEP::micrometer);
  m.attr("millimeter") = py::cast(CLHEP::millimeter);
  m.attr("mm") = py::cast(CLHEP::millimeter);
  m.attr("centimeter") = py::cast(CLHEP::centimeter);
  m.attr("cm") = py::cast(CLHEP::centimeter);
  m.attr("meter") = py::cast(CLHEP::meter);
  m.attr("m") = py::cast(CLHEP::meter);
  m.attr("kilometer") = py::cast(CLHEP::kilometer);
  m.attr("km") = py::cast(CLHEP::kilometer);

  m.attr("radian") = py::cast(CLHEP::radian);
  m.attr("rad") = py::cast(CLHEP::rad);
  m.attr("milliradian") = py::cast(CLHEP::milliradian);
  m.attr("mrad") = py::cast(CLHEP::milliradian);
  m.attr("degree") = py::cast(CLHEP::degree);
  m.attr("deg") = py::cast(CLHEP::degree);

  m.attr("picosecond") = py::cast(CLHEP::picosecond);
  m.attr("ps") = py::cast(CLHEP::picosecond);
  m.attr("nanosecond") = py::cast(CLHEP::nanosecond);
  m.attr("ns") = py::cast(CLHEP::nanosecond);
  m.attr("microsecond") = py::cast(CLHEP::microsecond);
  m.attr("us") = py::cast(CLHEP::microsecond);
  m.attr("millisecond") = py::cast(CLHEP::millisecond);
  m.attr("ms") = py::cast(CLHEP::millisecond);
  m.attr("second") = py::cast(CLHEP::second);
  m.attr("s") = py::cast(CLHEP::second);


  m.attr("c_light") = py::cast(CLHEP::c_light);
}
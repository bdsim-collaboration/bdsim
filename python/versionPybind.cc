#include <string>

#include <pybind11/pybind11.h>
namespace py = pybind11;

PYBIND11_MODULE(version, m) {
  m.attr("BDSIM_VERSION") = "@BDSIM_VERSION@";
  m.attr("BDSIM_MAJOR_VERSION") = "@BDSIM_MAJOR_VERSION@";
  m.attr("BDSIM_MINOR_VERSION") = "@BDSIM_MINOR_VERSION@";
  m.attr("BDSIM_PATCH_VERSION") = "@BDSIM_PATCH_LEVEL@";
  m.attr("VERSION_SHA1") = "@VERSION_SHA1@";
  m.attr("ROOT_VERSION") = "@ROOT_VERSION@";
  m.attr("CLHEP_VERSION") = "@CLHEP_VERSION@";
  m.attr("HEPMC3_VERSION") = "@HEPMC3_VERSION@";
  m.attr("HDF5_VERSION") = "@HDF5_VERSION@";
}
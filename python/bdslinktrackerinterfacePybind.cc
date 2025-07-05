//
// Created by Stewart Boogert on 07/06/2025.
//

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

// #define CACHE_PTRS 1

#include "BDSLinkTrackerInterface.hh"

template <typename T>
T* make_ptr(py::array_t<T> &arr) {
  auto buf = arr.request();
  T* ptr = static_cast<T*>(buf.ptr);
  return ptr;
}

template <typename T>
inline void set_element(py::array_t<T> &arr, int index, T value) {
  T* ptr = make_ptr<T>(arr);
  ptr[index] = value;
}

PYBIND11_MODULE(bdslinktrackerinterface, m) {
  py::class_<BDSLinkTrackerInterface>(m,"BDSLinkTrackerInterface")
      .def_static("GetInstance", [](std::string bdsimConfigFile,
                                    int referenceParticlePDG,
                                    double referenceKineticEnergy,
                                    double relativeEnergyCut,
                                    int seed,
                                    int referenceIonCharge,
                                    bool batchMode) {
        return BDSLinkTrackerInterface::GetInstance(bdsimConfigFile,
                                                    referenceParticlePDG,
                                                    referenceKineticEnergy,
                                                    relativeEnergyCut,
                                                    seed,
                                                    referenceIonCharge,
                                                    batchMode);
    },
                                    py::arg("bdsimConfigFileIn") = "trackerInterface.gmad",
                                    py::arg("referenceParticlePDG") =11,
                                    py::arg("referenceKineticEnergy") = 100,
                                    py::arg("relativeEnergyCut") = 0.01,
                                    py::arg("seed") = 1234,
                                    py::arg("referenceIonCharge") = 1,
                                    py::arg("batchMode") = true,
                                    py::return_value_policy::reference)
      .def_static("GetInstance", []() {return BDSLinkTrackerInterface::GetInstance();},
                  py::return_value_policy::reference)
      .def("TrackXSuite",[](BDSLinkTrackerInterface *link,
                            int iElement,
                            py::object particles) {
        py::print("Element BDSIM:",iElement);
        py::print("Particles:", particles);

        py::array_t<double> x = py::cast<py::array_t<double>>(particles.attr("x"));
        py::array_t<double> y = py::cast<py::array_t<double>>(particles.attr("y"));

        // fill BDSLinkBunch


#ifdef CACHE_PTRS
        double *x_ptr = make_ptr<double>(x);
        double *y_ptr = make_ptr<double>(y);
#endif

        // run n particles

        // get sampler data

        // change state of existing particles
        set_element<double>(x, 0, 1000);


        // add extra particles of products

        // clean BDSLinkBunch of particles
      });
}
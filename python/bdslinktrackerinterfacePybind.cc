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
#include "BDSIMLink.hh"
#include "BDSLinkBunch.hh"

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

void TrackXSuite(BDSLinkTrackerInterface *tracker_interface, py::object particles);
void TrackRFTrack(BDSLinkTrackerInterface *tracker_interface, py::object particles);

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
      .def("GetBunchLink",&BDSLinkTrackerInterface::GetBunchLink,py::return_value_policy::reference)
      .def("GetBDSIMLink",&BDSLinkTrackerInterface::GetBDSIMLink,py::return_value_policy::reference)
      .def("TrackXSuite",[](BDSLinkTrackerInterface *tracker_interface,
                            py::object particles) {
        TrackXSuite(tracker_interface, particles);
      })
      .def("TrackRFTrack",[](BDSLinkTrackerInterface *tracker_interface, py::object bunch6d) {
        TrackRFTrack(tracker_interface, bunch6d);
      });
}


void TrackXSuite(BDSLinkTrackerInterface *tracker_interface, py::object particles) {
  py::print("Particles:", particles);

  py::array_t<double> x = py::cast < py::array_t < double >> (particles.attr("x"));
  py::array_t<double> y = py::cast < py::array_t < double >> (particles.attr("y"));


  // fill BDSLinkBunch
  auto bunch = tracker_interface->GetBunchLink();
  for (int i = 0; i < 10; i++) {
    py::print(i);
    tracker_interface->AddParticle(0, 0, // x, y
                                   0, 0, // xp, yp
                                   0, 0, // ct, deltap
                                   1, 1, // chi, chargeRatio
                                   0, // s
                                   i, 11); // trackID, parent;
  }

#ifdef CACHE_PTRS
  double *x_ptr = make_ptr<double>(x);
  double *y_ptr = make_ptr<double>(y);
#endif

  // run n particles
  auto link = tracker_interface->GetBDSIMLink();
  link->BeamOn(10);

  // get sampler data
  //link->SamplerHits();

  // change state of existing particles
  set_element<double>(x, 0, 1000);

  // add extra particles of products

  // clean BDSLinkBunch of particles
}

void TrackRFTrack(BDSLinkTrackerInterface *tracker_interface, py::object bunch6d) {
  py::print("Bunch6d::", bunch6d);
  auto size_method = bunch6d.attr("size");
  py::print("Bunch6d::size",size_method());

  auto bdsim_link = tracker_interface->GetBDSIMLink();
  auto bunch_link = tracker_interface->GetBunchLink();

  int nparticle = bunch6d.attr("size")().cast<int>();

  for(int i = 0; i < nparticle ;i++) {
    auto p = bunch6d.attr("get_particle")(i);
    py::print(i);
    tracker_interface->AddParticle(0,0, // x, y
                                   0,0, // xp, yp
                                   0,0, // ct, deltap
                                   1,1, // chi, chargeRatio
                                   0, // s
                                   i, 11); // trackID, parent;
  }
  bdsim_link->BeamOn(nparticle);
}
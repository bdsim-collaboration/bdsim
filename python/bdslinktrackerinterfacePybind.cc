/* 
Beam Delivery Simulation (BDSIM) Copyright (C) BDSIM Collaboration, 2001 - 2026.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
//
// Created by Stewart Boogert on 07/06/2025.
//
#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "Randomize.hh"

#include "BDSLinkTrackerInterface.hh"
#include "BDSIMLink.hh"
#include "BDSLinkBunch.hh"
#include "BDSSamplerCustom.hh"
#include "BDSParser.hh"

#include "BDSLinkParticleBatch.hh"
#include "BDSLinkSamplerParticleBatch.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>
#include <unordered_map>

namespace
{
  using DoubleArray =
      py::array_t<double, py::array::c_style | py::array::forcecast>;

  using Int64Array =
      py::array_t<long long, py::array::c_style | py::array::forcecast>;

  struct RFTrackBunchConversionResult
  {
    BDSLinkParticleBatch batch;

    // Maps an RF-Track particle ID to its original bunch index.
    std::unordered_map<int, std::size_t> rfTrackIndexByParticleID;
  };

  DoubleArray RequireMatrix(const py::object& object,
                            py::ssize_t numberOfColumns,
                            const std::string& description)
  {
    DoubleArray array = DoubleArray::ensure(object);

    if (!array)
      {
        throw std::runtime_error(
            description + " is not convertible to a contiguous float64 array");
      }

    if (array.ndim() != 2 || array.shape(1) != numberOfColumns)
      {
        throw std::runtime_error(description + " must have shape (N, " +
                                 std::to_string(numberOfColumns) + ")");
      }

    return array;
  }

  DoubleArray RequireColumn(const py::object& object,
                            py::ssize_t expectedRows,
                            const std::string& description)
  {
    DoubleArray array = DoubleArray::ensure(object);

    if (!array)
      {
        throw std::runtime_error(description +
                                 " is not convertible to float64");
      }

    const bool validOneDimensional =
        array.ndim() == 1 && array.shape(0) == expectedRows;

    const bool validColumn = array.ndim() == 2 &&
                             array.shape(0) == expectedRows &&
                             array.shape(1) == 1;

    if (!validOneDimensional && !validColumn)
      {
        throw std::runtime_error(description + " has an unexpected shape");
      }

    return array;
  }

  BDSLinkParticleBatch ConvertBunch6dCoordinates(const py::object& bunch)
  {
    py::object phaseObject =
        bunch.attr("get_phase_space")("%x %xp %y %yp %t %P", "all");

    DoubleArray phase =
        RequireMatrix(phaseObject, 6, "RF-Track Bunch6d phase space");

    const py::ssize_t n = phase.shape(0);
    auto input = phase.unchecked<2>();

    BDSLinkParticleBatch batch;

    batch.x.resize(n);
    batch.y.resize(n);
    batch.px.resize(n);
    batch.py.resize(n);
    batch.pz.resize(n);
    batch.t.resize(n);
    batch.s.assign(n, 0.0);
    batch.particleID.resize(n);
    batch.pdgID.assign(n, 0);

    for (py::ssize_t i = 0; i < n; ++i)
      {
        const double xMm = input(i, 0);
        const double xpMrad = input(i, 1);
        const double yMm = input(i, 2);
        const double ypMrad = input(i, 3);
        const double tMmC = input(i, 4);
        const double pMeVc = input(i, 5);

        const double xpRad = xpMrad * 1.0e-3;
        const double ypRad = ypMrad * 1.0e-3;

        const double directionNorm =
            std::sqrt(1.0 + xpRad * xpRad + ypRad * ypRad);

        if (!std::isfinite(directionNorm) || directionNorm <= 0.0 ||
            !std::isfinite(pMeVc) || pMeVc <= 0.0)
          {
            throw std::runtime_error("Invalid RF-Track Bunch6d momentum "
                                     "at row " +
                                     std::to_string(i));
          }

        const double pzMeVc = pMeVc / directionNorm;

        batch.x[i] = xMm;
        batch.y[i] = yMm;
        batch.px[i] = xpRad * pzMeVc;
        batch.py[i] = ypRad * pzMeVc;
        batch.pz[i] = pzMeVc;

        // RF-Track: mm/c.
        // CLHEP::c_light: mm/ns.
        batch.t[i] = tMmC / CLHEP::c_light;
      }

    return batch;
  }

  double RFTrackBunchMassMeVc2(const py::object& bunch)
  {
    for (const char* methodName : {"get_mass", "get_m0"})
      {
        if (!py::hasattr(bunch, methodName))
          {
            continue;
          }

        try
          {
            const double mass = py::cast<double>(bunch.attr(methodName)());

            if (std::isfinite(mass) && mass > 0.0)
              {
                return mass;
              }
          }
        catch (const py::error_already_set&)
          {
            // Try next method
          }
      }

    try
      {
        DoubleArray masses =
            RequireColumn(bunch.attr("get_phase_space")("%m", "all"),
                          py::cast<py::ssize_t>(bunch.attr("size")()),
                          "RF-Track particle masses");

        const double* data = masses.data();

        if (masses.size() > 0 && std::isfinite(data[0]) && data[0] > 0.0)
          {
            return data[0];
          }
      }
    catch (const py::error_already_set&)
      {
      }

    throw std::runtime_error("Unable to determine RF-Track bunch mass");
  }

  BDSLinkParticleBatch ConvertBunch6dTCoordinates(const py::object& bunch)
  {
    DoubleArray phase = RequireMatrix(bunch.attr("get_phase_space")(),
                                      6,
                                      "RF-Track Bunch6dT phase space");

    const py::ssize_t n = phase.shape(0);

    DoubleArray t0 = RequireColumn(bunch.attr("get_phase_space")("%t0", "all"),
                                   n,
                                   "RF-Track Bunch6dT t0");

    const double massMeVc2 = RFTrackBunchMassMeVc2(bunch);

    auto input = phase.unchecked<2>();
    const double* t0Data = t0.data();

    BDSLinkParticleBatch batch;

    batch.x.resize(n);
    batch.y.resize(n);
    batch.px.resize(n);
    batch.py.resize(n);
    batch.pz.resize(n);
    batch.t.resize(n);
    batch.s.assign(n, 0.0);
    batch.particleID.resize(n);
    batch.pdgID.assign(n, 0);

    for (py::ssize_t i = 0; i < n; ++i)
      {
        const double xMm = input(i, 0);
        const double pxMeVc = input(i, 1);
        const double yMm = input(i, 2);
        const double pyMeVc = input(i, 3);
        const double zMm = input(i, 4);
        const double pzMeVc = input(i, 5);
        const double t0MmC = t0Data[i];

        const double momentumMeVc =
            std::sqrt(pxMeVc * pxMeVc + pyMeVc * pyMeVc + pzMeVc * pzMeVc);

        if (!std::isfinite(momentumMeVc) || momentumMeVc <= 0.0)
          {
            throw std::runtime_error("Invalid RF-Track Bunch6dT momentum "
                                     "at row " +
                                     std::to_string(i));
          }

        const double totalEnergyMeV =
            std::sqrt(momentumMeVc * momentumMeVc + massMeVc2 * massMeVc2);

        const double beta = momentumMeVc / totalEnergyMeV;

        const double timeMmC = t0MmC - zMm / beta;

        batch.x[i] = xMm;
        batch.y[i] = yMm;
        batch.px[i] = pxMeVc;
        batch.py[i] = pyMeVc;
        batch.pz[i] = pzMeVc;
        batch.t[i] = timeMmC / CLHEP::c_light;
      }

    return batch;
  }

  void FillParticleMetadata(const py::object& bunch,
                            BDSLinkParticleBatch& batch)
  {
    const py::ssize_t n = static_cast<py::ssize_t>(batch.Size());

    bool idsExtracted = false;

    try
      {
        Int64Array ids =
            Int64Array::ensure(bunch.attr("get_phase_space")("%id", "all"));

        if (ids && ids.size() == n)
          {
            const long long* data = ids.data();
            std::unordered_set<int> uniqueIDs;

            for (py::ssize_t i = 0; i < n; ++i)
              {
                const long long value = data[i];

                if (value < std::numeric_limits<int>::min() ||
                    value > std::numeric_limits<int>::max())
                  {
                    throw std::runtime_error("RF-Track particle ID is outside "
                                             "the C++ int range");
                  }

                const int id = static_cast<int>(value);

                if (!uniqueIDs.insert(id).second)
                  {
                    throw std::runtime_error(
                        "RF-Track particle IDs are not unique");
                  }

                batch.particleID[i] = id;
              }

            idsExtracted = true;
          }
      }
    catch (const py::error_already_set&)
      {
        idsExtracted = false;
      }

    for (py::ssize_t i = 0; i < n; ++i)
      {
        py::object particle = bunch.attr("get_particle")(i);

        if (!idsExtracted)
          {
            batch.particleID[i] = py::cast<int>(particle.attr("id"));
          }

        if (py::hasattr(particle, "pdg_id"))
          {
            batch.pdgID[i] = py::cast<int>(particle.attr("pdg_id"));
          }
      }
  }

  BDSLinkParticleBatch
  RFTrackBunchToBDSLinkParticleBatch(const py::object& bunch,
                                     const int fallbackPDGID)
  {
    if (fallbackPDGID == 0)
      {
        throw py::value_error("pdg_id must be non-zero; "
                              "use 2212 for protons");
      }
    if (!py::hasattr(bunch, "get_phase_space") || !py::hasattr(bunch, "size"))
      {
        throw std::invalid_argument("Expected an RF-Track Bunch6d or Bunch6dT");
      }

    const std::string className =
        py::str(bunch.attr("__class__").attr("__name__"));

    BDSLinkParticleBatch batch;

    if (className == "Bunch6dT")
      {
        batch = ConvertBunch6dTCoordinates(bunch);
      }
    else if (className == "Bunch6d")
      {
        batch = ConvertBunch6dCoordinates(bunch);
      }
    else
      {
        throw std::invalid_argument("Unsupported RF-Track bunch type \"" +
                                    className + "\"");
      }

    FillParticleMetadata(bunch, batch);

    if (batch.pdgID.size() != batch.x.size())
      {
        throw py::value_error("Internal error: pdgID and particle arrays "
                              "have different sizes");
      }

    for (int& pdgID : batch.pdgID)
      {
        if (pdgID == 0)
          {
            pdgID = fallbackPDGID;
          }
      }

    batch.Validate();

    return batch;
  }

  RFTrackBunchConversionResult
  RFTrackBunchToBDSLinkConversionResult(const py::object& bunch,
                                        int fallbackPDGID)
  {
    RFTrackBunchConversionResult conversion;

    conversion.batch = RFTrackBunchToBDSLinkParticleBatch(bunch, fallbackPDGID);

    const std::size_t numberOfParticles = conversion.batch.Size();

    conversion.rfTrackIndexByParticleID.reserve(numberOfParticles);

    for (std::size_t index = 0; index < numberOfParticles; ++index)
      {
        const int particleID = conversion.batch.particleID[index];

        const auto insertion =
            conversion.rfTrackIndexByParticleID.emplace(particleID, index);

        if (!insertion.second)
          {
            throw py::value_error("Duplicate RF-Track particle ID " +
                                  std::to_string(particleID));
          }
      }

    return conversion;
  }

  void RequireFiniteParticleValue(double value,
                                  const char* quantityName,
                                  std::size_t particleIndex)
  {
    if (!std::isfinite(value))
      {
        throw py::value_error("Particle " + std::to_string(particleIndex) +
                              " has a non-finite " + quantityName + " value");
      }
  }
  RFTrackBunchConversionResult
  LoadRFTrackBunchIntoTracker(BDSLinkTrackerInterface& trackerInterface,
                              const py::object& bunch,
                              int fallbackPDGID)
  {
    RFTrackBunchConversionResult conversion =
        RFTrackBunchToBDSLinkConversionResult(bunch, fallbackPDGID);

    BDSLinkParticleBatch& batch = conversion.batch;

    const std::size_t n = batch.Size();

    if (batch.Empty())
      {
        throw py::value_error("Cannot load an empty RF-Track bunch");
      }

    batch.Validate();

    // Avoid repeating the Geant4 lookup for particles
    // belonging to the same species.
    std::unordered_set<int> validatedPDGIDs;
    validatedPDGIDs.reserve(n);

    for (std::size_t i = 0; i < n; ++i)
      {
        RequireFiniteParticleValue(batch.x[i], "x", i);

        RequireFiniteParticleValue(batch.y[i], "y", i);

        RequireFiniteParticleValue(batch.t[i], "t", i);

        RequireFiniteParticleValue(batch.s[i], "s", i);

        RequireFiniteParticleValue(batch.px[i], "px", i);

        RequireFiniteParticleValue(batch.py[i], "py", i);

        RequireFiniteParticleValue(batch.pz[i], "pz", i);

        const double momentumSquared = batch.px[i] * batch.px[i] +
                                       batch.py[i] * batch.py[i] +
                                       batch.pz[i] * batch.pz[i];

        if (!std::isfinite(momentumSquared))
          {
            throw py::value_error("Particle " + std::to_string(i) +
                                  " has an invalid momentum magnitude");
          }

        if (momentumSquared <= 0.0)
          {
            throw py::value_error("Particle " + std::to_string(i) +
                                  " has zero momentum");
          }

        const int particlePDGID = batch.pdgID[i];

        if (particlePDGID == 0)
          {
            throw py::value_error("Particle " + std::to_string(i) +
                                  " has an undefined PDG ID");
          }

        const auto insertion = validatedPDGIDs.insert(particlePDGID);

        if (insertion.second)
          {
            G4ParticleDefinition* particleDefinition =
                trackerInterface.GetParticleDefinition(particlePDGID);

            if (!particleDefinition)
              {
                throw py::value_error("Particle " + std::to_string(i) +
                                      " has unknown PDG ID " +
                                      std::to_string(particlePDGID));
              }

            G4ParticleDefinition* tableDefinition =
                G4ParticleTable::GetParticleTable()->FindParticle(
                    particlePDGID);

            if (!tableDefinition)
              {
                throw py::value_error("PDG ID " +
                                      std::to_string(particlePDGID) +
                                      " is not available to "
                                      "BDSLinkTrackerInterface::AddParticle");
              }
          }
      }

    trackerInterface.ClearData();

    trackerInterface.AddParticles(batch.x,
                                  batch.y,
                                  batch.px,
                                  batch.py,
                                  batch.pz,
                                  batch.t,
                                  batch.s,
                                  batch.particleID,
                                  batch.pdgID);

    if (trackerInterface.GetBunchLink()->Size() != n)
      {
        throw std::runtime_error("BDSIMLink did not load the expected "
                                 "number of RF-Track particles");
      }

    return conversion;
  }

  BDSLinkSamplerParticleBatch BDSLinkSamplerHitsToParticleBatch(
      const BDSHitsCollectionSamplerLink& samplerHits)
  {
    BDSLinkSamplerParticleBatch output;

    const std::size_t n = samplerHits.entries();

    BDSLinkParticleBatch& particles = output.particles;

    particles.x.resize(n);
    particles.y.resize(n);
    particles.px.resize(n);
    particles.py.resize(n);
    particles.pz.resize(n);
    particles.t.resize(n);
    particles.s.resize(n);
    particles.particleID.resize(n);
    particles.pdgID.resize(n);

    output.parentID.resize(n);
    output.trackID.resize(n);
    output.eventID.resize(n);
    output.weight.resize(n);

    for (std::size_t i = 0; i < n; ++i)
      {
        const BDSHitSamplerLink* hit = samplerHits[i];

        if (!hit)
          {
            throw std::runtime_error("Null BDSIM sampler hit at index " +
                                     std::to_string(i));
          }

        const double directionNormSquared = hit->coords.xp * hit->coords.xp +
                                            hit->coords.yp * hit->coords.yp +
                                            hit->coords.zp * hit->coords.zp;

        if (!std::isfinite(directionNormSquared) || directionNormSquared <= 0.0)
          {
            throw std::runtime_error(
                "Invalid momentum direction in sampler hit " +
                std::to_string(i));
          }

        if (!std::isfinite(hit->momentum) || hit->momentum < 0.0)
          {
            throw std::runtime_error("Invalid momentum in sampler hit " +
                                     std::to_string(i));
          }

        const double inverseDirectionNorm =
            1.0 / std::sqrt(directionNormSquared);

        particles.x[i] = hit->coords.x;

        particles.y[i] = hit->coords.y;

        particles.px[i] = hit->momentum * hit->coords.xp * inverseDirectionNorm;

        particles.py[i] = hit->momentum * hit->coords.yp * inverseDirectionNorm;

        particles.pz[i] = hit->momentum * hit->coords.zp * inverseDirectionNorm;

        particles.t[i] = hit->coords.T;

        // This is the sampler-local longitudinal coordinate.
        // It is not the accumulated beamline position.
        particles.s[i] = hit->coords.s;

        particles.particleID[i] = hit->externalParticleID;

        particles.pdgID[i] = hit->pdgID;

        output.parentID[i] = hit->externalParentID;

        output.trackID[i] = hit->trackID;

        output.eventID[i] = hit->eventID;

        output.weight[i] = hit->coords.weight;
      }

    output.Validate();

    return output;
  }
}

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

void TrackXSuite(BDSLinkTrackerInterface *tracker_interface,
                 int iElement,
                 std::string elementName,
                 py::object particles,
                 float referenceKineticEnergy);

void TrackRFTrack(
  BDSLinkTrackerInterface* trackerInterface,
  int elementIndex,
  py::object bunch6d);


PYBIND11_MODULE(bdslinktrackerinterface, m) {
  py::class_<BDSLinkTrackerInterface>(m,"BDSLinkTrackerInterface")
      .def_static("GetInstance", [](std::string bdsimConfigFile,
                                    int referenceParticlePDG,
                                    double referenceKineticEnergy,
                                    double relativeEnergyCut,
                                    int seed,
                                    int referenceIonCharge,
                                    bool batchMode,
                                    bool no_neutral_particles) {
        auto* obj = BDSLinkTrackerInterface::GetInstance(bdsimConfigFile,
                                                         referenceParticlePDG,
                                                         referenceKineticEnergy,
                                                         relativeEnergyCut,
                                                         seed,
                                                         referenceIonCharge,
                                                         batchMode);
        obj->SetNoNeutralParticles(no_neutral_particles);
        return obj;
    },            py::arg("bdsimConfigFileIn") = "trackerInterface.gmad",
                  py::arg("referenceParticlePDG") =11,
                  py::arg("referenceKineticEnergy") = 100,
                  py::arg("relativeEnergyCut") = 0.01,
                  py::arg("seed") = 1234,
                  py::arg("referenceIonCharge") = 1,
                  py::arg("batchMode") = true,
                  py::arg("no_neutral_particles") = true,
                  py::return_value_policy::reference)

    .def_static("GetInstance", [](BDSParser* parser,
                                  int referenceParticlePDG,
                                  double referenceKineticEnergy,
                                  double relativeEnergyCut,
                                  int seed,
                                  int referenceIonCharge,
                                  bool batchMode,
                                  bool no_neutral_particles) {
      auto* obj = BDSLinkTrackerInterface::GetInstance(parser,
                                                       referenceParticlePDG,
                                                       referenceKineticEnergy,
                                                       relativeEnergyCut,
                                                       seed,
                                                       referenceIonCharge,
                                                       batchMode);
      obj->SetNoNeutralParticles(no_neutral_particles);
      return obj;
    }, py::arg("parser"),
       py::arg("referenceParticlePDG") =11,
       py::arg("referenceKineticEnergy") = 100,
       py::arg("relativeEnergyCut") = 0.01,
       py::arg("seed") = 1234,
       py::arg("referenceIonCharge") = 1,
       py::arg("batchMode") = true,
       py::arg("no_neutral_particles") = true,
       py::return_value_policy::reference)

    .def_static("GetInstance", []() {return BDSLinkTrackerInterface::GetInstance();},
                py::return_value_policy::reference)
    .def("Reset", &BDSLinkTrackerInterface::Reset)
    .def("PrepareBDSParticleDefinition", &BDSLinkTrackerInterface::PrepareBDSParticleDefinition)
    .def("GetReferenceParticleDefinition", [](BDSLinkTrackerInterface &ti) {
      return ti.GetReferenceParticleDefinition();
    }, py::return_value_policy::reference)
    .def("SetReferenceParticleDefinition",&BDSLinkTrackerInterface::SetReferenceParticleDefinition, py::arg("particleDefinition"))
    .def("GetBDSIMConfigFile", &BDSLinkTrackerInterface::GetBDSIMConfigFile)
    .def("GetReferenceParticlePDG", &BDSLinkTrackerInterface::GetReferenceParticlePDG)
    .def("GetReferenceParticleKineticEnergy", &BDSLinkTrackerInterface::GetReferenceParticleKineticEnergy)
    .def("GetRelativeEnergyCut", &BDSLinkTrackerInterface::GetRelativeEnergyCut)
    .def("GetSeed", &BDSLinkTrackerInterface::GetSeed)
    .def("GetReferenceIonCharge", &BDSLinkTrackerInterface::GetReferenceIonCharge)
    .def("GetBatchMode", &BDSLinkTrackerInterface::GetBatchMode)
    .def("GetMinimumKineticEnergy", &BDSLinkTrackerInterface::GetMinimumKineticEnergy)
    .def("SetNoNeutralParticles", &BDSLinkTrackerInterface::SetNoNeutralParticles)
    .def("GetNoNeutralParticles", &BDSLinkTrackerInterface::GetNoNeutralParticles)
    .def("GetBunchLink",&BDSLinkTrackerInterface::GetBunchLink,py::return_value_policy::reference)
    .def("GetBDSIMLink",&BDSLinkTrackerInterface::GetBDSIMLink,py::return_value_policy::reference)
    .def("AddParticle",[](BDSLinkTrackerInterface &ti, double x, double y, double px, double py,
                          double ct, double deltap, double chi, double chargeRatio,
                          double s, int trackid, int pdgid) {
      ti.AddParticle(x,y, px, py, ct, deltap, chi, chargeRatio, s, trackid, pdgid);
    }, py::arg("x"), py::arg("y"), py::arg("xp"), py::arg("yp"),
    py::arg("ct"), py::arg("deltap"), py::arg("chi"), py::arg("chargeRatio"),
    py::arg("s"), py::arg("trackid"), py::arg("pdgid"))
    .def("AddParticle",[](BDSLinkTrackerInterface &ti, double x, double y, double px, double py,
                          double pz, double t, double s,
                          int trackid, int pdgid) {
      ti.AddParticle(x, y, px, py, pz, t, s, trackid, pdgid);
    }, py::arg("x"), py::arg("y"), py::arg("px"), py::arg("py"), py::arg("pz"),
    py::arg("t"), py::arg("s"), py::arg("trackid"), py::arg("pdgid"))
    .def("AddParticles",[](BDSLinkTrackerInterface &ti,
                           std::vector<double> &x,
                           std::vector<double> &y,
                           std::vector<double> &xp,
                           std::vector<double> &yp,
                           std::vector<double> &ct,
                           std::vector<double> &deltap,
                           std::vector<double> &chi,
                           std::vector<double> &chargeRatio,
                           std::vector<double> &s,
                           std::vector<int> &trackid,
                           std::vector<int> &pdgid) {
      ti.AddParticles(x,y,xp,yp, ct, deltap, chi, chargeRatio, s, trackid, pdgid);
    }, py::arg("x"), py::arg("y"), py::arg("xp"), py::arg("yp"),
    py::arg("ct"), py::arg("deltap"), py::arg("chi"), py::arg("chargeRatio"),
    py::arg("s"), py::arg("trackid"), py::arg("pdgid"))
    .def("AddParticles",[](BDSLinkTrackerInterface &ti,
                           std::vector<double> x,
                           std::vector<double> y,
                           std::vector<double> px,
                           std::vector<double> py,
                           std::vector<double> pz,
                           std::vector<double> t,
                           std::vector<double> s,
                           std::vector<int> trackid,
                           std::vector<int> pdgid) {
      ti.AddParticles(x,y,px,py,pz,t,s,trackid,pdgid);
    }, py::arg("x"), py::arg("y"), py::arg("px"), py::arg("py"), py::arg("pz"),
    py::arg("t"), py::arg("s"), py::arg("trackid"), py::arg("pdgid"))
    .def("ClearData",&BDSLinkTrackerInterface::ClearData)
    .def("GetParticleDefinition", &BDSLinkTrackerInterface::GetParticleDefinition,py::keep_alive<1, 2>())
    .def("GetParticlePDGMass",&BDSLinkTrackerInterface::GetParticlePDGMass)
    .def("GetParticlePDGCharge", &BDSLinkTrackerInterface::GetParticlePDGCharge)
    .def("GetChargeRatio", &BDSLinkTrackerInterface::GetChargeRatio)
    .def("GetMassRatio",&BDSLinkTrackerInterface::GetMassRatio)
    .def("GetChi", &BDSLinkTrackerInterface::GetChi)
    .def("TrackXSuite",[](BDSLinkTrackerInterface *tracker_interface,
        int iElement,
        std::string elementName,
        py::object particles,
        float referenceKineticEnergy) {
      TrackXSuite(tracker_interface, iElement, elementName, particles, referenceKineticEnergy);
    })
    .def(
      "_LoadRFTrackBunch",
      [](BDSLinkTrackerInterface& trackerInterface,
         const py::object& bunch,
         int pdgID)
      {
        const RFTrackBunchConversionResult conversion =
          LoadRFTrackBunchIntoTracker(
            trackerInterface,
            bunch,
            pdgID
          );

        return conversion.batch.Size();
      },
      py::arg("bunch"),
      py::arg("pdg_id")
    )
    .def(
      "_GetSamplerParticleBatch",
      [](BDSLinkTrackerInterface& trackerInterface)
      {
        auto* samplerHits =
          trackerInterface
            .GetBDSIMLink()
            ->SamplerHits();

        if (!samplerHits)
        {
          throw std::runtime_error(
            "No sampler hits are available; "
            "call BeamOn first"
          );
        }

        BDSLinkSamplerParticleBatch output =
          BDSLinkSamplerHitsToParticleBatch(
            *samplerHits
          );

        const BDSLinkParticleBatch& particles =
          output.particles;

        py::dict result;

        result["x_mm"]          = particles.x;
        result["y_mm"]          = particles.y;
        result["px_mevc"]       = particles.px;
        result["py_mevc"]       = particles.py;
        result["pz_mevc"]       = particles.pz;
        result["t_ns"]          = particles.t;
        result["s_local_mm"]    = particles.s;
        result["particle_ids"]  = particles.particleID;
        result["pdg_ids"]       = particles.pdgID;
        result["parent_ids"]    = output.parentID;
        result["track_ids"]     = output.trackID;
        result["event_ids"]     = output.eventID;
        result["weights"]       = output.weight;

        return result;
      }
    )
    .def(
      "TrackRFTrack",
      [](BDSLinkTrackerInterface* trackerInterface,
         int elementIndex,
         py::object bunch6d)
      {
        TrackRFTrack(
          trackerInterface,
          elementIndex,
          bunch6d
        );
      },
      py::arg("element_index"),
      py::arg("bunch")
    );

  m.def(
    "_ConvertRFTrackBunchToBDSLinkBatch",
    [](py::object bunch,
       int pdgID)
    {
      const RFTrackBunchConversionResult conversion =
        RFTrackBunchToBDSLinkConversionResult(
          bunch,
          pdgID
        );

      const BDSLinkParticleBatch& batch =
        conversion.batch;

      py::dict result;

      result["x_mm"]         = batch.x;
      result["y_mm"]         = batch.y;
      result["px_mevc"]      = batch.px;
      result["py_mevc"]      = batch.py;
      result["pz_mevc"]      = batch.pz;
      result["t_ns"]         = batch.t;
      result["s_mm"]         = batch.s;
      result["particle_ids"] = batch.particleID;
      result["pdg_ids"]      = batch.pdgID;

      py::dict indexByParticleID;

      for (
        const auto& entry :
        conversion.rfTrackIndexByParticleID
      )
        {
          indexByParticleID[
            py::int_(entry.first)
          ] = py::int_(entry.second);
        }

      result["rftrack_index_by_particle_id"] =
        indexByParticleID;

      return result;
    },
    py::arg("bunch"),
    py::arg("pdg_id")
  );

}

void TrackXSuite(BDSLinkTrackerInterface *tracker_interface,
                            int iElement,
                            std::string elementName,
                            py::object particles,
                            float referenceKineticEnergy) {
    py::print("Element BDSIM:",iElement);
    py::print("Particles:", particles);

    py::array_t<double> x = py::cast<py::array_t<double>>(particles.attr("x"));
    py::array_t<double> y = py::cast<py::array_t<double>>(particles.attr("y"));
    py::array_t<double> px = py::cast<py::array_t<double>>(particles.attr("px"));
    py::array_t<double> py = py::cast<py::array_t<double>>(particles.attr("py"));
    py::array_t<double> ct = py::cast<py::array_t<double>>(particles.attr("zeta"));
    py::array_t<double> deltap = py::cast<py::array_t<double>>(particles.attr("delta"));
    py::array_t<double> chi = py::cast<py::array_t<double>>(particles.attr("chi"));
    py::array_t<double> charge_ratio = py::cast<py::array_t<double>>(particles.attr("charge_ratio"));
    py::array_t<double> massratio = py::cast<py::array_t<double>>(particles.attr("mass_ratio"));
    py::array_t<double> s = py::cast<py::array_t<double>>(particles.attr("s"));
    py::array_t<int64_t> pdgid = py::cast<py::array_t<int64_t>>(particles.attr("pdg_id"));
    py::array_t<int64_t> trackid = py::cast<py::array_t<int64_t>>(particles.attr("particle_id"));
    py::array_t<int64_t> parentid = py::cast<py::array_t<int64_t>>(particles.attr("parent_particle_id"));
    py::array_t<int64_t> state = py::cast<py::array_t<int64_t>>(particles.attr("state"));
    py::array_t<int64_t> at_element = py::cast<py::array_t<int64_t>>(particles.attr("at_element"));
    py::array_t<int64_t> at_turn = py::cast<py::array_t<int64_t>>(particles.attr("at_turn"));

    // fill BDSLinkBunch
    auto bunch = tracker_interface->GetBunchLink();
    auto ref = tracker_interface->GetReferenceParticleDefinition();
    // bunch->AddParticle();

    // Fast pointers to numpy arrays
    auto get = [&](const char* name) {
        return py::cast<py::array_t<double>>(particles.attr(name)).mutable_data();
    };
    auto geti = [&](const char* name) {
        return py::cast<py::array_t<int64_t>>(particles.attr(name)).mutable_data();
    };

    double* x_ptr = get("x");
    double* y_ptr = get("y");
    double* px_ptr = get("px");
    double* py_ptr = get("py");
    double* zeta_ptr = get("zeta");
    double* delta_ptr = get("delta");
    double* chi_ptr = get("chi");
    double* charge_ratio_ptr = get("charge_ratio");
    double* s_ptr = get("s");
    int64_t* pdgid_ptr = geti("pdg_id");
    int64_t* trackid_ptr = geti("particle_id");
    int64_t* state_ptr = geti("state");
    int64_t* at_element_ptr = geti("at_element");
    int64_t* at_turn_ptr = geti("at_turn");

    // Add particles to the bunch to be tracked
    ssize_t n = py::cast<py::array_t<double>>(particles.attr("x")).size();
    auto& active_state = tracker_interface->GetParticleActiveState();
    int n_active = 0;
    for (ssize_t i = 0; i < n; ++i) {
        if (state_ptr[i] < 1) {  // there can be different types of alive states in xsuite
            active_state.push_back(false);
            continue;
        }
        else {
            G4double q = charge_ratio_ptr[i] * ref->Charge();
            G4double mass_ratio = charge_ratio_ptr[i] / chi_ptr[i];
            G4double p = ref->Momentum() * (delta_ptr[i] + 1) * mass_ratio;

            auto partDef = tracker_interface->PrepareBDSParticleDefinition(pdgid_ptr[i],
                                                                           /*totalEnergy */ 0,
                                                                           p,
                                                                           /* kineticEnergy */ 0,
                                                                           q);
            G4double t = - zeta_ptr[i] * CLHEP::m / (ref->Beta() * CLHEP::c_light);

            G4double oneplusdelta = (1 + delta_ptr[i]) * mass_ratio;
            G4double xp = px_ptr[i] / oneplusdelta;
            G4double yp = py_ptr[i] / oneplusdelta;
            G4double zp = BDSBunch::CalculateZp(xp, yp, 1);

            BDSParticleCoordsFull coords(x_ptr[i] * CLHEP::m,
                                         y_ptr[i] * CLHEP::m,
                                         0,
                                         xp, yp, zp,
                                         t,
                                         0,
                                         partDef->TotalEnergy(),
                                         1);
            active_state.push_back(true);
            ++n_active;
            bunch->AddParticle(partDef, coords, trackid_ptr[i], trackid_ptr[i]);
        }
    }

    // run n particles
    auto link = tracker_interface->GetBDSIMLink();
    std::cout << "beam on with: " << bunch->Size() << std::endl;
    link->SelectLinkElement(elementName);
    link->BeamOn((G4int)bunch->Size());
    std::cout << "finished tracking" << std::endl;
    // get sampler data
    const BDSHitsCollectionSamplerLink* hits = link->SamplerHits();
    std::cout << "sampled hits: " << hits->GetSize() << std::endl;
    size_t hitsCount = 0;
    if (hits)
    {
        hitsCount = hits->GetSize();
    }
    else
    {
        // There were no hits - check if there were any active particles at all coming in
        if (!bunch->Size())
        {
            bunch->GetNextParticleLocal();
        }
    }
    // Count the number of secondary particles
    int secondaryCount = 0;
    for (size_t i = 0; i < hitsCount; i++)
    {
        auto hit = (*hits)[i];
        if (hit->externalParticleID != hit->externalParentID) { secondaryCount = secondaryCount+1; }
    }

    // The output arrays have slots for all particles, regardless of lost or not, and for secondary particles
    size_t output_size = x.size();

    // Loop through the particles in the *original* bunch - the primaries
    size_t hits_index = 0;
    bool prim_survied = false;
    // double sum_deltaplusone_sec = 0.0;
    double sum_secondary_energy = 0.0;

    //size_t prod_write_index = active_state.size();
    size_t prod_write_index = n_active;
    for (size_t i=0; i < active_state.size(); i++){
        if (!active_state.at(i)){
            continue; // This was an inactive particle that hasn't been processed, do not change it
        }

        auto part = bunch->GetNextParticle(); // Advance through the bunch
        auto prim_part_id = bunch->CurrentExternalParticleID(); // Get the ID of the primary particle

        // Now start looping over the hits - the particles to be returned to the tracker
        // These can be primary or secondary particles. Each primary can produce 0, 1, or 2+ products
        // The products need to be sorted to keep the array order - surviving primary particles are all
        // filled in first. If a primary didn't survive, keep the original coordinates and make it inactive.
        // The hits are ordered by primary event, so just need one loop.
        while (hits_index < hitsCount)
        {
            BDSHitSamplerLink* hit = (*hits)[hits_index];
            if (hit->externalParentID != prim_part_id) { // The hits corresponding to the current primary are exhausted
                break;
            }

            const BDSParticleCoordsFull &coords = hit->coords;

            double qratio = hit->charge / ref->Charge();
            double mratio = hit->mass / ref->Mass();
            double dp = (hit->momentum / mratio - ref->Momentum()) / ref->Momentum();

            double collLength = link->GetArcLengthOfLinkElement(iElement);
            /// Need to compensate for the geometry construction in BDSIM
            /// There is a safety margin that is added to the collimator legnth
            double collMargin = 2.5 * BDSSamplerCustom::ChordLength();
            double zt = ref->Beta() * CLHEP::c_light * ((collLength + collMargin) / (CLHEP::c_light * ref->Beta()) - coords.T);
            double oneplusdelta = (1 + dp) * mratio;

            auto track_id = hit->externalParticleID;
            auto parent_id = hit->externalParentID;
            auto pdg_id = hit->pdgID;

            if (track_id == parent_id){
                if (i >= output_size) {
                    std::cerr << "Primary index " << i << " out of bounds! Check capacity of particles object" << std::endl;
                    std::abort();
                }
                // This is a primary particle as its parent is itself
                prim_survied = true;

                set_element<double>(s, i, s_ptr[i] + collLength / CLHEP::m);
                set_element<double>(x, i, coords.x / CLHEP::m);
                set_element<double>(px, i, coords.xp * oneplusdelta);
                set_element<double>(y, i, coords.y / CLHEP::m);
                set_element<double>(py, i, coords.yp * oneplusdelta);
                set_element<double>(ct, i, zt / CLHEP::m);
                set_element<double>(deltap, i, dp);
                set_element<int64_t>(pdgid, i, pdg_id);
                set_element<double>(charge_ratio, i, qratio);
                set_element<int64_t>(parentid, i, track_id); // should be set to parent_id, not trackid
                set_element<int64_t>(state, i, 1);

            }
            else
            {
                if (qratio == 0 && tracker_interface->GetNoNeutralParticles()) {
                    std::cout << "Skipping neutral secondary with parent ID " << parent_id << std::endl;
                    ++hits_index;
                    continue;
                }
                if (prod_write_index >= output_size) {
                    std::cerr << "Secondary index " << prod_write_index << " out of bounds! Check the capacity of the particles object" << std::endl;
                    std::abort();
                }
                // Secondary particles are populated in newly allocated arrays
                set_element<double>(s, prod_write_index, s_ptr[i] + collLength / CLHEP::m);
                set_element<double>(x, prod_write_index, coords.x / CLHEP::m);
                set_element<double>(px, prod_write_index, coords.xp * oneplusdelta);
                set_element<double>(y, prod_write_index, coords.y / CLHEP::m);
                set_element<double>(py, prod_write_index, coords.yp * oneplusdelta);
                set_element<double>(ct, prod_write_index, zt / CLHEP::m);
                set_element<double>(deltap, prod_write_index, dp);
                set_element<int64_t>(pdgid, prod_write_index, pdg_id);
                set_element<int64_t>(at_element, prod_write_index, at_element_ptr[i] + 1); // remove
                set_element<int64_t>(at_turn, prod_write_index, at_turn_ptr[i]); // remove
                set_element<double>(charge_ratio, prod_write_index, qratio);
                set_element<int64_t>(parentid, prod_write_index, parent_id); // should be set to parent_id, not trackid
                set_element<int64_t>(state, prod_write_index, 1);
                double chi_val = qratio / mratio;
                set_element<double>(chi, prod_write_index, chi_val);

                sum_secondary_energy += std::sqrt(std::pow(hit->momentum,2) + std::pow(hit->mass,2));
                prod_write_index++;
            }

            hits_index++;
        }

        if (!prim_survied) // Primary didn't survive - set inactive
        {
            if (i >= output_size) {
                std::cerr << "Primary index " << i << " out of bounds! Check the capacity of the particles object" << std::endl;
                std::abort();
            }
            set_element<int64_t>(state, i, -333); // inactive
            // Correct the energy of the lost primary particle to account for the production of secondaries
            // The effective delta is such that the lost particle has the effective delta
            // which corresponds to the energy in - energy out for this primary

            // reconstruct the incoming primary particle energy
            // G4double qprim = charge_ratio_ptr[i] * ref->Charge();
            G4double mass_ratio_prim = charge_ratio_ptr[i] / chi_ptr[i];
            G4double p_prim = ref->Momentum() * (delta_ptr[i] + 1) * mass_ratio_prim;
            G4double mass_prim = mass_ratio_prim * ref->Mass();
            G4double energy_prim = std::sqrt(std::pow(p_prim, 2) + std::pow(mass_prim, 2));

            // compute the effective delta
            G4double energy_diff = energy_prim - sum_secondary_energy;
            G4double p_eff;
            G4double squared_energy_mass_diff = std::pow(energy_diff, 2) - std::pow(mass_prim, 2);
            if (squared_energy_mass_diff < 0){
                // This means that the total energy escaping includes part of the rest mass of
                // the primary. Tolerate the error for now, as otherwise need to adjust also the
                // mass and PDG id of the lost primary particle
                p_eff = 0;
            }
            else
            {
                p_eff = std::sqrt(squared_energy_mass_diff);
            }
            G4double delta_eff = (p_eff / mass_ratio_prim - ref->Momentum()) / ref->Momentum();
            set_element<double>(deltap, i, delta_eff);
        }
        prim_survied = false; // reset for next particle
        sum_secondary_energy = 0.0;
    }
    std::cout << "ref energy is: " << ref->Momentum() << std::endl;
    std::cout << "ref energy submitted in GeV is: " << referenceKineticEnergy / CLHEP::GeV << std::endl;
    // clean BDSLinkBunch of particles
    tracker_interface->ClearData();
}

void TrackRFTrack(BDSLinkTrackerInterface* trackerInterface,
                  int elementIndex,
                  py::object bunch6d)
{
  if (!trackerInterface)
    {
      throw std::invalid_argument("BDSLinkTrackerInterface pointer is null");
    }

  if (elementIndex < 0)
    {
      throw py::value_error("element_index must be non-negative");
    }

  if (!py::hasattr(bunch6d, "S"))
    {
      throw py::type_error("TrackRFTrack requires an RF-Track Bunch6d "
                           "with an S coordinate");
    }

  const double elementSStartM = py::cast<double>(bunch6d.attr("S"));

  if (!std::isfinite(elementSStartM))
    {
      throw py::value_error("RF-Track bunch.S must be finite");
    }

  py::print("TrackRFTrack> Bunch6d::", bunch6d);

  py::print("TrackRFTrack> Bunch6d::size", bunch6d.attr("size")());

  auto* bdsimLink = trackerInterface->GetBDSIMLink();

  if (!bdsimLink)
    {
      throw std::runtime_error("BDSIMLink is not available");
    }

  bdsimLink->SelectLinkElement(elementIndex);

  const double elementArcLengthMm =
      bdsimLink->GetArcLengthOfLinkID(elementIndex);

  if (!std::isfinite(elementArcLengthMm) || elementArcLengthMm < 0.0)
    {
      throw std::runtime_error("Selected BDSIM link element has an "
                               "invalid arc length");
    }
  const double samplerSGlobalM = elementSStartM + elementArcLengthMm / CLHEP::m;

  if (!std::isfinite(samplerSGlobalM))
    {
      throw std::runtime_error("Calculated sampler S position is not finite");
    }
  const int fallbackPDGID = trackerInterface->GetReferenceParticlePDG();

  const RFTrackBunchConversionResult conversion =
      LoadRFTrackBunchIntoTracker(*trackerInterface, bunch6d, fallbackPDGID);

  const BDSLinkParticleBatch& loadedBatch = conversion.batch;

  const auto& rfTrackIndexByParticleID = conversion.rfTrackIndexByParticleID;

  const std::size_t numberLoaded = loadedBatch.Size();

  if (numberLoaded > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
      throw std::overflow_error("RF-Track bunch contains too many particles "
                                "for BDSIMLink::BeamOn");
    }

  const int numberOfParticles = static_cast<int>(numberLoaded);

  bdsimLink->BeamOn(numberOfParticles);

  auto* samplerHits = bdsimLink->SamplerHits();

  if (!samplerHits)
    {
      throw std::runtime_error("No BDSIM sampler hit collection is available "
                               "after BeamOn");
    }

  const BDSLinkSamplerParticleBatch samplerBatch =
      BDSLinkSamplerHitsToParticleBatch(*samplerHits);

  const BDSLinkParticleBatch& sampledParticles = samplerBatch.particles;

  // Initially mark every particle as not having reached
  // the output sampler. The output-sampler position is
  // used as a temporary loss position until exact BDSIM
  // loss coordinates are available.
  for (int i = 0; i < numberOfParticles; ++i)
    {
      py::object particle = bunch6d.attr("get_particle")(i);

      particle.attr("S_lost") = py::cast(samplerSGlobalM);

      const int particlePDGID = py::cast<int>(particle.attr("pdg_id"));

      // Record the fallback PDG ID actually used by BDSIM.
      if (particlePDGID == 0)
        {
          particle.attr("pdg_id") = py::cast(fallbackPDGID);
        }
    }

  py::object appendMethod = bunch6d.attr("append");

  std::vector<double> newParticleData =
      {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  for (std::size_t i = 0; i < sampledParticles.Size(); ++i)
    {
      const int parentParticleID = samplerBatch.parentID[i];

      const auto parentIndexIterator =
          rfTrackIndexByParticleID.find(parentParticleID);

      if (parentIndexIterator == rfTrackIndexByParticleID.end())
        {
          throw std::runtime_error("Sampler hit refers to unknown RF-Track "
                                   "parent particle ID " +
                                   std::to_string(parentParticleID));
        }

      const std::size_t parentIndex = parentIndexIterator->second;

      py::object particle = bunch6d.attr("get_particle")(parentIndex);

      const int sampledParticleID = sampledParticles.particleID[i];

      const int sampledPDGID = sampledParticles.pdgID[i];

      const bool isPrimary = sampledParticleID == parentParticleID;

      const double pxMeVc = sampledParticles.px[i];

      const double pyMeVc = sampledParticles.py[i];

      const double pzMeVc = sampledParticles.pz[i];

      const double momentumMeVc =
          std::hypot(std::hypot(pxMeVc, pyMeVc), pzMeVc);

      if (!std::isfinite(momentumMeVc) || momentumMeVc <= 0.0)
        {
          throw std::runtime_error("Sampler particle has invalid momentum "
                                   "at index " +
                                   std::to_string(i));
        }

      const double longitudinalDirection = pzMeVc / momentumMeVc;

      if (!std::isfinite(longitudinalDirection) ||
          std::abs(longitudinalDirection) <=
              std::numeric_limits<double>::epsilon())
        {
          throw std::runtime_error("Sampler particle has a zero or invalid "
                                   "longitudinal momentum direction at index " +
                                   std::to_string(i));
        }

      // RF-Track Bunch6d stores transverse slopes in mrad.
      const double xpMrad = 1.0e3 * pxMeVc / pzMeVc;

      const double ypMrad = 1.0e3 * pyMeVc / pzMeVc;

      if (isPrimary)
        {
          // The primary particle reached
          // the output sampler.
          particle.attr("x") = py::cast(sampledParticles.x[i]);

          particle.attr("y") = py::cast(sampledParticles.y[i]);

          particle.attr("xp") = py::cast(xpMrad);

          particle.attr("yp") = py::cast(ypMrad);

          particle.attr("Pc") = py::cast(momentumMeVc);

          particle.attr("S_lost") = py::cast(std::nan(""));

          // Synchronise the RF-Track particle with
          // the particle species reported by BDSIM.
          particle.attr("pdg_id") = py::cast(sampledPDGID);

          // TODO: Calculate and update the particle's
          // absolute time.
        }
      else
        {
          // Secondary particle produced by BDSIM.
          newParticleData[0] = sampledParticles.x[i];

          newParticleData[1] = xpMrad;

          newParticleData[2] = sampledParticles.y[i];

          newParticleData[3] = ypMrad;

          G4ParticleDefinition* particleDefinition =
              G4ParticleTable::GetParticleTable()->FindParticle(sampledPDGID);

          if (!particleDefinition)
            {
              throw std::runtime_error(
                  "Unable to find Geant4 particle with PDG ID " +
                  std::to_string(sampledPDGID));
            }

          newParticleData[6] = particleDefinition->GetPDGMass();

          newParticleData[7] = particleDefinition->GetPDGCharge();

          newParticleData[8] =
              py::cast<double>(particle.attr("N")) * samplerBatch.weight[i];

          const int insertionIndex = bunch6d.attr("size")().cast<int>();

          py::array_t<double> newParticleArray(newParticleData.size(),
                                               newParticleData.data());

          appendMethod(newParticleArray);

          particle = bunch6d.attr("get_particle")(insertionIndex);

          particle.attr("S_lost") = py::cast(std::nan(""));

          particle.attr("Pc") = py::cast(momentumMeVc);

          particle.attr("pdg_id") = py::cast(sampledPDGID);

          // TODO: Calculate the secondary particle's
          // absolute time.

          const double lifetime = particleDefinition->GetPDGLifeTime();

          const double mass = particleDefinition->GetPDGMass();

          if (lifetime < 0.0)
            {
              // A negative Geant4 lifetime represents
              // a stable particle.
              particle.attr("lifetime") =
                  std::numeric_limits<double>::infinity();
            }
          else if (mass > 0.0)
            {
              // TODO: Calculate and sample the
              // relativistically boosted lifetime.
              particle.attr("lifetime") =
                  std::numeric_limits<double>::infinity();
            }
        }
    }

  bunch6d.attr("S") = py::cast(samplerSGlobalM);
}

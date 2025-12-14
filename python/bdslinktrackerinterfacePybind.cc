/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

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

void TrackRFTrack(BDSLinkTrackerInterface *tracker_interface,
                  py::object particles);


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
    },
                                    py::arg("bdsimConfigFileIn") = "trackerInterface.gmad",
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
      .def("TrackRFTrack",[](BDSLinkTrackerInterface *tracker_interface, py::object bunch6d) {
        TrackRFTrack(tracker_interface, bunch6d);
      });
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

void TrackRFTrack(BDSLinkTrackerInterface *tracker_interface, py::object bunch6d) {
  py::print("TrackRFTrack> Bunch6d::", bunch6d);
  auto size_method = bunch6d.attr("size");
  py::print("TrackRFTrack> Bunch6d::size",size_method());

  auto bdsim_link = tracker_interface->GetBDSIMLink();

  auto refPDG = tracker_interface->GetReferenceParticleDefinition()->PDGID();
  // clear sampler hits (do this first and not at end as sampler data will
  // no longer available in python)
  bdsim_link->ClearSamplerHits();

  int nparticle = bunch6d.attr("size")().cast<int>();

  for(int i = 0; i < nparticle ;i++) {
    // py::print("TrackRFTrack> ",i);

    auto p = bunch6d.attr("get_particle")(i);
    auto x = py::cast<double>(p.attr("x"));
    auto y = py::cast<double>(p.attr("y"));
    auto p4v = (py::cast<py::array_t<double>>(p.attr("get_four_momentum")())).unchecked<2>();
    // TODO time needs to calculated wrt to reference particle
    // auto t = py::cast<double>(p.attr("t"));
    auto pdgID = py::cast<int>(p.attr("pdg_id")); // initiall this will be zero and add particle will set to reference particle

    tracker_interface->AddParticle(x*CLHEP::mm, y*CLHEP::mm, // x, y
                                   p4v(1,0), p4v(2,0), p4v(3,0), // px, px, pz
                                   0, // ct
                                   0, // s
                                   i, pdgID); // trackID, parent;
    if (pdgID == 0) {
      p.attr("pdg_id") = py::cast(refPDG);
    }
  }
  bdsim_link->BeamOn(nparticle);

  // loop over sampler hits and update bunch
  auto sh = bdsim_link->SamplerHits();

  // set all bunch particles as if they didn't make it (s)
  //auto endpoint = (*sh)[0]->coords.s;
  auto endpoint = 0.1;
  for(int i=0; i<nparticle; i++) {
    auto p = bunch6d.attr("get_particle")(i);
    p.attr("S_lost") = py::cast(endpoint);
  }


  auto append_method = bunch6d.attr("append");
  std::vector<double> new_particle_data = {0, 0, 0, 0, 0, 0, 0, 0, 0};

  for(std::size_t i=0; i<sh->entries(); i++) {
    auto h = (*sh)[i];
    // std::cout << i << " " << h->externalParentID << std::endl;
    auto externalID = h->externalParentID;
    auto trackID = h->trackID;
    auto p = bunch6d.attr("get_particle")(externalID);
    if(trackID == 1) { // existing partucke
      p.attr("x") = py::cast(h->coords.x);
      p.attr("y") = py::cast(h->coords.y);
      p.attr("xp") = py::cast(h->coords.xp);
      p.attr("yp") = py::cast(h->coords.yp);
      p.attr("S_lost") = py::cast(std::nan(""));
      // TODO colulate absolute T for particle
    }
    else { // new particle

      new_particle_data[0] = h->coords.x;
      new_particle_data[1] = h->coords.xp;
      new_particle_data[2] = h->coords.y;
      new_particle_data[3] = h->coords.yp;

      // look up particle mass and charge
      G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(h->pdgID);
      new_particle_data[6] = particle->GetPDGMass();
      new_particle_data[7] = particle->GetPDGCharge();
      new_particle_data[8] = py::cast<double>(p.attr("N"))*h->coords.weight;

      // append bdsim generated particle
      int idx_insert = bunch6d.attr("size")().cast<int>();
      py::array_t<double> new_particle_arr(new_particle_data.size(), new_particle_data.data());
      append_method(new_particle_arr);

      // need to set particle as not lost
      p = bunch6d.attr("get_particle")(idx_insert);
      p.attr("S_lost") = py::cast(std::nan(""));

      // set particle momentum
      p.attr("Pc") = py::cast(h->momentum);

      // set particle pdg
      p.attr("pdg_id") = py::cast(h->pdgID);

      // TODO colulate absolute T for new particles particle
      // set particle time

      // set created particle lifetime for RF Track
      auto lifetime = particle->GetPDGLifeTime();
      auto mass = particle->GetPDGMass();
      if (lifetime < 0) { // set infinite lifetime
        p.attr("lifetime") = std::numeric_limits<double>::infinity();
      }
      else if(mass > 0) { // need to have non zero mass
        //vauto boosted_lifetime = h->coords.totalEnergy/mass*lifetime;
        // auto sampled_lifetime = -boosted_lifetime * std::log(G4UniformRand());
        // std::cout << lifetime << " " << h->coords.totalEnergy << " " << mass << std::endl;
        // p.attr("lifetime") = py::cast(boosted_lifetime);

        p.attr("lifetime") = std::numeric_limits<double>::infinity();
      }
    }
  }
}
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
#ifndef BDSPHYSICSMUON_H
#define BDSPHYSICSMUON_H

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief High energy muon processes.
 * 
 * Constructs:
 * Gamma to Muon pair.
 * e+ annihilation to Muon pair
 * e+ annihilation to hadrons.
 * Muon multiple scattering.
 * Muon ionisation.
 * Muon bremsstrahlung.
 * Muon pair production from Muons.
 *
 * Uses the G4PhysicsListHelper to set and order processes.
 *
 */

class BDSPhysicsMuon: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsMuon();
  explicit BDSPhysicsMuon(G4bool emWillBeUsed);
  virtual ~BDSPhysicsMuon();
  /// Construct all leptons, photons (inc optical), and pion +- just in case.
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();

private:

  /// Whether em physics will be used, which provides processes that would be duplicate
  /// here.
  G4bool emWillBeUsed;
};
#endif

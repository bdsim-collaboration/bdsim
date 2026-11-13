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
#ifndef BDSPHYSICSGAMMATOMUMU_H
#define BDSPHYSICSGAMMATOMUMU_H

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

/**
 * @brief High energy muon processes.
 * 
 * Constructs gamma to Muon pair only.
 *
 * @author Laurie Nevay
 */

class BDSPhysicsGammaToMuMu: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsGammaToMuMu();
  virtual ~BDSPhysicsGammaToMuMu();

  /// Construct required particles.
  virtual void ConstructParticle();

  /// Construct and attach the processes to the relevant particles.
  virtual void ConstructProcess();
};
#endif

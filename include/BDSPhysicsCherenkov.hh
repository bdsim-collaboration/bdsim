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
#ifndef BDSPHYSICSCHERENKOV_H
#define BDSPHYSICSCHERENKOV_H

#include "BDSSingleUse.hh"

#include "G4Types.hh"
#include "G4VPhysicsConstructor.hh"

/**
 * @brief Cherenkov physics process constructor.
 *
 * @author Laurie Nevay
 */

class BDSPhysicsCherenkov: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsCherenkov() = delete;
  BDSPhysicsCherenkov(G4int maxPhotonsPerStepIn,
											G4double maxBetaChangePerStepIn);
  virtual ~BDSPhysicsCherenkov();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:
  G4int maxPhotonsPerStep;
  G4double maxBetaChangePerStep;
};

#endif

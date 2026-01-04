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
#ifndef BDSBUNCHRING_H
#define BDSBUNCHRING_H 

#include "BDSBunch.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief A bunch distribution that produces an uncorrelated random
 * uniform distribution along a circle in phase space.
 * 
 * @author Stewart Boogert
 */

class BDSBunchRing: public BDSBunch
{
public: 
  BDSBunchRing(); 
  virtual ~BDSBunchRing(); 
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchRing& operator=(const BDSBunchRing&) = delete;
  BDSBunchRing(BDSBunchRing&) = delete;
  /// @}
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
                          const GMAD::Beam& beam,
                          const BDSBunchType& distrType,
                          G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
                          const G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual BDSParticleCoordsFull GetNextParticleLocal();
  
protected:
  G4double rMin;
  G4double rMax;
  /// @{ Used for a range of rp.
  G4double rpMin;
  G4double rpMax;
  /// @}
  G4double rpSingleValue;
  G4bool anyNonZeroRp;
  G4bool useRpRange;
};

#endif

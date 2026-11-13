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
#ifndef BDSBUNCHCIRCLE_H
#define BDSBUNCHCIRCLE_H 

#include "BDSBunch.hh"

/**
 * @brief An uncorrelated uniform random distribution within a circle in each dimension.
 * 
 * @author Stewart Boogert
 */

class BDSBunchCircle: public BDSBunch
{
public: 
  BDSBunchCircle(); 
  virtual ~BDSBunchCircle(); 
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchCircle& operator=(const BDSBunchCircle&) = delete;
  BDSBunchCircle(BDSBunchCircle&) = delete;
  /// @}
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual BDSParticleCoordsFull GetNextParticleLocal();
  
protected:  
  G4double envelopeR; 
  G4double envelopeRp; 
  G4double envelopeT;
  G4double envelopeE;
};

#endif

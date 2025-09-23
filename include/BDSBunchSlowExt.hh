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
#ifndef BDSBUNCHSLOWEXT_H
#define BDSBUNCHSLOWEXT_H

#include "BDSBunch.hh"

/**
 * @brief A bunch distribution with a linear sweep in momentum in time.
 *
 * Represents slow extraction beams.
 * 
 * @author Laurie Nevay
 */

class BDSBunchSlowExt: public BDSBunch
{ 
public:
  BDSBunchSlowExt();
  virtual ~BDSBunchSlowExt();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchSlowExt& operator=(const BDSBunchSlowExt&) = delete;
  BDSBunchSlowExt(BDSBunchSlowExt&) = delete;
  /// @}
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
													const GMAD::Beam& beam,
													const BDSBunchType& distrType,
													G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
													const G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual BDSParticleCoordsFull GetNextParticleLocal();

	/// Generate the random shift for this call.
	void GetDeltas(G4double& dt, G4double& dE);
  
protected:
  G4double dTStart;
  G4double dTStop;
  G4double dPStart;
  G4double dPStop;
  G4bool applyGradient;
  G4double dT;
	G4double dEStart;
  G4double dE;
};

#endif

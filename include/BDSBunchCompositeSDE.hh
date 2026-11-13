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
#ifndef BDSBUNCHCOMPOSITESDE_H
#define BDSBUNCHCOMPOSITESDE_H 
#include "BDSBunch.hh"

/**
 * @brief A distribution that allows mixing of three different 
 * distributions in each primary dimension.
 * 
 * Mixed by space, direction and energy ("SDE")
 * 
 * @author Laurie Nevay
 */

class BDSBunchCompositeSDE: public BDSBunch
{
public:
  BDSBunchCompositeSDE(); 
  virtual ~BDSBunchCompositeSDE();
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual void SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn);

  virtual BDSParticleCoordsFull GetNextParticleLocal();

protected:
  BDSBunch* sBunch;
  BDSBunch* dBunch;
  BDSBunch* eBunch;
};

#endif

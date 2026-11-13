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
#ifndef BDSBUNCHCOMPOSITE_H
#define BDSBUNCHCOMPOSITE_H 

#include "globals.hh"

#include "BDSBunch.hh"

/**
 * @brief A distribution that allows mixing of three different 
 * distributions in each primary dimension.
 * 
 * @author Stewart Boogert
 */

class BDSBunchComposite: public BDSBunch
{
public:
  BDSBunchComposite(); 
  virtual ~BDSBunchComposite();
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual void SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn);

  virtual BDSParticleCoordsFull GetNextParticleLocal();

protected:
  BDSBunch* xBunch;
  BDSBunch* yBunch;
  BDSBunch* zBunch;
};

#endif

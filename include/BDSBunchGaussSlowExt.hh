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
#ifndef BDSBUNCHGAUSSSLOWEXT_H
#define BDSBUNCHGAUSSSLOWEXT_H

#include "BDSBunch.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

#include <vector>

class BDSBunchGauss;
class BDSBunchSlowExt;

namespace GMAD
{
  class Beam;
}

/**
 * @brief Gaussian with a P-T correlation for slow extraction added on top.
 * 
 * @author Laurie Nevay
 */

class BDSBunchGaussSlowExt: public BDSBunch
{
public:
  BDSBunchGaussSlowExt();
  virtual ~BDSBunchGaussSlowExt();
  
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
                          const GMAD::Beam& beam,
                          const BDSBunchType& distrType,
                          G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
                          const G4double beamlineS = 0);

  virtual void CheckParameters();

  /// Either draw from the vector of already created points or fire fresh
  /// from the matrix.
  virtual BDSParticleCoordsFull GetNextParticleLocal();

  virtual void BeginOfRunAction(G4int numberOfEvents,
                                G4bool batchMode);
  
protected:
  BDSBunchGauss* gauss;
  BDSBunchSlowExt* slowext;
};

#endif

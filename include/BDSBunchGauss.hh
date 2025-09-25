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
#ifndef BDSBUNCHGAUSS_H
#define BDSBUNCHGAUSS_H

#include "BDSBunchGaussBase.hh"

#include "G4Transform3D.hh"

namespace GMAD
{
  class Beam;
}

/**
 * @brief A 6D Gaussian distribution based on a covariance matrix.
 * 
 * @author Stewart Boogert
 */

class BDSBunchGauss: public BDSBunchGaussBase
{
public:
	BDSBunchGauss();
  virtual ~BDSBunchGauss(){;}
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
													const GMAD::Beam& beam,
													const BDSBunchType& distrType,
													G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
													const G4double beamlineS = 0);
};

#endif

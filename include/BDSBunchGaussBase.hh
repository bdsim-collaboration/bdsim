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
#ifndef BDSBUNCHGAUSSBASE_H
#define BDSBUNCHGAUSSBASE_H

#include "BDSBunch.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

#include <vector>

namespace CLHEP
{
  class HepRandomEngine;
  class RandMultiGauss;
}

namespace GMAD
{
  class Beam;
}

/**
 * @brief Common functionality for a 6D Gaussian distribution.
 * 
 * @author Stewart Boogert
 */

class BDSBunchGaussBase: public BDSBunch
{
public:
  BDSBunchGaussBase() = delete;
  explicit BDSBunchGaussBase(const G4String& nameIn);
  virtual ~BDSBunchGaussBase();
  
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);

  /// Called at the beginning of a run. Override here to call PreGenerateEvents that
  /// will generate all coordinates and subtract the sample mean.
  virtual void BeginOfRunAction(G4int numberOfEvents,
                                G4bool batchMode);

  /// Either draw from the vector of already created points or fire fresh
  /// from the matrix.
  virtual BDSParticleCoordsFull GetNextParticleLocal();

  inline G4bool OffsetSampleMean() const {return offsetSampleMean;}
  
protected:
  /// Create multidimensional Gaussian random number generator
  /// for Twiss and Gauss. Can change sigma matrix to make non-definite.
  CLHEP::RandMultiGauss* CreateMultiGauss(CLHEP::HepRandomEngine& anEngine,
					  const CLHEP::HepVector& mu,
					  CLHEP::HepSymMatrix&    sigma);
  
  /// Pre-generate all the particle coordinates and subtract the sample mean.
  void PreGenerateEvents(G4int nGenerate);

  /// Fire random number generator and get coordinates. Can be overloaded if required.
  virtual BDSParticleCoordsFull GetNextParticleLocalCoords();

  CLHEP::HepVector    meansGM;
  CLHEP::HepSymMatrix sigmaGM;

  /// Randon number generator with sigma matrix and mean.
  CLHEP::RandMultiGauss* gaussMultiGen;
  
  G4bool offsetSampleMean; ///< Whether to offset the sample mean.

  /// @{ Holder for pre-calculated coordinates.
  std::vector<G4double> x0_v, xp_v, y0_v, yp_v, z0_v, zp_v,E_v,t_v,weight_v;
  /// @}
  G4int iPartIteration; ///< Iterator for reading out pre-calculate coordinates

  /// Convenience vector of vectors for clearing up.
  std::vector<std::vector<G4double>* > coordinates;
};

#endif

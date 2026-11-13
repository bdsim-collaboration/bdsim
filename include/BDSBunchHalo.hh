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
#ifndef BDSBUNCHHALO_H
#define BDSBUNCHHALO_H

#include "BDSBunch.hh"

namespace CLHEP {
  class RandFlat;
}

/**
 * @brief A halo distribution based on both twiss parameters and sigmas.
 *
 * @author Stewart Boogert, Simon Williams
 */

class BDSBunchHalo: public BDSBunch
{
public:
  BDSBunchHalo();
  virtual ~BDSBunchHalo();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchHalo& operator=(const BDSBunchHalo&) = delete;
  BDSBunchHalo(BDSBunchHalo&) = delete;
  /// @}
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  virtual void CheckParameters();
  virtual BDSParticleCoordsFull GetNextParticleLocal();
  
private: 
  /// @{ Twiss parameter
  G4double alphaX;
  G4double alphaY;
  G4double betaX;
  G4double betaY;
  G4double emitX;
  G4double emitY;
  G4double gammaX;
  G4double gammaY;
  G4double sigmaX;
  G4double sigmaY;
  G4double sigmaXp;
  G4double sigmaYp;
  /// @}

  G4double haloNSigmaXInner;
  G4double haloNSigmaXOuter;
  G4double haloNSigmaYInner;
  G4double haloNSigmaYOuter;
  G4double haloXCutInner;
  G4double haloYCutInner;
  G4double haloXCutOuter;
  G4double haloYCutOuter;
  G4double haloXpCutInner;
  G4double haloYpCutInner;
  G4double haloXpCutOuter;
  G4double haloYpCutOuter;
  G4double haloPSWeightParameter;
  G4String weightFunction;

  G4double emitInnerX;
  G4double emitInnerY;
  G4double emitOuterX;
  G4double emitOuterY;

  G4double xMax;
  G4double yMax;
  G4double xpMax;
  G4double ypMax;
};

#endif

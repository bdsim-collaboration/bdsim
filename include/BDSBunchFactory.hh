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
#ifndef BDSBUNCHFACTORY_H
#define BDSBUNCHFACTORY_H

#include "BDSBunchType.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class BDSBunch;
class BDSParticleDefinition;

namespace GMAD
{
  class Beam;
}

/**
 * @brief Factory class for BDSBunch.
 * 
 * Instantiate the specific type of bunch distribution (class),
 * get the corresponding parameters from the gmad parser info
 * and attach to the initialised random number generator.
 * 
 * @author Jochem Snuverink
 */

class BDSBunchFactory
{
public:
  /// factory method
  static BDSBunch* CreateBunch(const BDSParticleDefinition* beamParticle,
			       const GMAD::Beam&            beam,
			       const G4Transform3D&         beamlineTransform = G4Transform3D::Identity,
			       G4double                     beamlineS = 0,
			       G4bool                       generatePrimariesOnlyIn = false);

  static BDSBunch* CreateBunch(const BDSParticleDefinition* beamParticle,
			       BDSBunchType                 distrType,
			       const GMAD::Beam&            beam,
			       const G4Transform3D&         beamlineTransform = G4Transform3D::Identity,
			       G4double                     beamlineS = 0,
			       G4bool                       generatePrimariesOnlyIn = false);
};

#endif

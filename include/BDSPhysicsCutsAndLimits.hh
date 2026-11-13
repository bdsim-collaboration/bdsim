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
#ifndef BDSCUTSANDLIMITS_H
#define BDSCUTSANDLIMITS_H

#include "BDSSingleUse.hh"

#include "G4VPhysicsConstructor.hh"

#include <set>

class BDSProcessUserSpecialCutsPDG;
class G4StepLimiter;
class G4UserSpecialCuts;

/**
 * @brief Physics processes required for user tracking limits.
 */

class BDSPhysicsCutsAndLimits: public G4VPhysicsConstructor, public BDSSingleUse
{
public:
  BDSPhysicsCutsAndLimits() = delete;
  explicit BDSPhysicsCutsAndLimits(const std::set<G4int>& pdgsToExcludeFromCuts = {});
  virtual ~BDSPhysicsCutsAndLimits();
  /// @{ Assignment and copy constructor not implemented nor used
  BDSPhysicsCutsAndLimits& operator=(const BDSPhysicsCutsAndLimits&) = delete;
  BDSPhysicsCutsAndLimits(BDSPhysicsCutsAndLimits&) = delete;
  /// @}

  /// Construct gamma, e+- and proton - the minimum this sets limits for.
  virtual void ConstructParticle();

  /// Construct and attach step limiter and cuts processes.
  virtual void ConstructProcess();

private:
  G4StepLimiter*     stepLimiter;  ///< Step limit process for MaxAllowedStep.
  G4UserSpecialCuts* specialCuts;  ///< Process for all other limits.
  BDSProcessUserSpecialCutsPDG* bdsSpecialCuts;
  G4bool useParticleExclusionFromCuts;
};
#endif

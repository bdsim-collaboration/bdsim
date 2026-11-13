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
#ifndef BDSBEAMLINEINTEGRAL_H
#define BDSBEAMLINEINTEGRAL_H

#include "BDSParticleDefinition.hh"

#include "G4Types.hh"

namespace GMAD
{
  struct Element;
}

/**
 * @brief A class that holds the current integrated quantities along a beam line.
 *
 * Used to hold accumulated quantities like the current total arc length, synchronous
 * global time and current particle rigidity / momentum along the beam line.
 * 
 * @author Laurie Nevay
 */

class BDSBeamlineIntegral
{
public:
  BDSBeamlineIntegral() = delete;
  explicit BDSBeamlineIntegral(const BDSParticleDefinition& incomingParticle,
			       G4double T0In = 0,
			       G4double integratedArcLength = 0,
			       G4bool   integrateKineticEnergyIn = true);
  ~BDSBeamlineIntegral();

  /// Increment all quantities in this class by adding this element
  /// onto the beam line.
  void Integrate(const GMAD::Element& componentAsDefined);
  
  /// Provide the next synchronous T at the centre if we were to add on this new
  /// element to the beam line (i.e. integrate it). Doesn't change the current
  /// contents though. Constructs a copy of this instance, does the calculation,
  /// then does away with the copy.
  G4double ProvideSynchronousTAtCentreOfNextElement(const GMAD::Element* el) const;

  /// Provide the next synchronous T at the middle if we were to add on this new
  /// element. Similar to above function.
  G4double ProvideSynchronousTAtEndOfNextElement(const GMAD::Element* el) const;

  G4double MomentumAtStartOfLastElement() const {return designParticle.Momentum();}
  G4double KineticEnergyAtStartOfLastElement() const {return designParticle.KineticEnergy();}
  
  G4bool   integrateKineticEnergy; ///< On by default, but allow control to not integrate it for old behaviour.
  G4double synchronousTAtEnd;
  G4double synchronousTAtMiddleOfLastElement;
  G4double arcLength;
  BDSParticleDefinition designParticle;
  G4bool   changeOfEnergyEncountered;
  G4int    rigidityCount;  ///< Which change in rigidity we're at. Starts at 0, +1 after each change. Used for naming.
};

#endif

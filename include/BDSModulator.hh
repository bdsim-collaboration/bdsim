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
#ifndef BDSMODULATOR_H
#define BDSMODULATOR_H

#include "G4ThreeVector.hh"
#include "G4Types.hh"

/**
 * @brief Base class for a modulator.
 * 
 * A modulator returns a numerical factor based on x,y,z,t,eventNumber.
 *
 * Turn number can also be used and should be accessed through BDSGlobalConstants
 * in the derived class that would wish to use this (static) variable.
 * 
 * @author Fabian Metzger
 */

class BDSModulator
{
public:
  BDSModulator() = default;
  virtual ~BDSModulator() = default;

  /// Derived class must implement this. T is global, others are presumed
  /// to be local coordinates.
  virtual G4double Factor(const G4ThreeVector& xyz,
                          G4double T) const = 0;

  /// Setter for the static counter so we can trace where it's used.
  static void SetEventIndex(G4int eventIndexIn);
  
  /// Each derived class should override this.
  virtual G4bool VariesWithTime() const = 0;
  
  /// Must return the smallest spatial
  virtual G4double RecommendedMaxStepLength() const = 0;
  
protected:
  static G4int eventIndex;
};

#endif

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
#ifndef BDSSCORERTIMEFILTER_H
#define BDSSCORERTIMEFILTER_H

#include "G4String.hh"
#include "G4Types.hh"
#include "G4VSDFilter.hh"

#include "CLHEP/Units/SystemOfUnits.h"

class G4Step;

/**
 * @brief Filter for time value for a sensitive detector.
 *
 * @author Robin Tesse
 */

class BDSSDFilterTime: public G4VSDFilter
{
public:
  /// Constructor with default values.
  explicit BDSSDFilterTime(const G4String& name,
			   G4double lowerLimitIn = 0.0,
			   G4double upperLimitIn = 1*CLHEP::s);
  
  virtual ~BDSSDFilterTime(){;}

  /// Whether a step will be accepted or rejected.
  virtual G4bool Accept(const G4Step*) const;
  
private:
  /// No default constructor required.
  BDSSDFilterTime() = delete;
  G4double lowerLimit;
  G4double upperLimit;
};

#endif

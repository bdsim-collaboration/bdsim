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
#ifndef BDSMODULATORLINEART_H
#define BDSMODULATORLINEART_H

#include "BDSModulator.hh"

#include "G4Types.hh"

/**
 * @brief Top-hat modulator as a function of T
 * 
 * factor = amplitudeScale, T0 <= T <= T1 ; else 0
 * 
 * @author Laurie Nevay
 */

class BDSModulatorLinearT: public BDSModulator
{
public:
  BDSModulatorLinearT() = delete;
  explicit BDSModulatorLinearT(G4double T0,
			       G4double T1,
			       G4double slopeIn      = 0.0,
                   G4double yInterceptIn = 1.0);
  virtual ~BDSModulatorLinearT() = default;

  /// Returns value of equation in brief at the top of this file.
  virtual G4double Factor(const G4ThreeVector& xyz,
                          G4double T) const;
  
  virtual G4bool VariesWithTime() const {return true;}
  
  /// Return difference in T0, T1 / 20.
  virtual G4double RecommendedMaxStepLength() const;

private:
  G4double T0;
  G4double T1;
  G4double slope;
  G4double yIntercept;
};

#endif

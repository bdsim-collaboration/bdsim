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
#ifndef BDSMODULATORSINT_H
#define BDSMODULATORSINT_H
#include "BDSModulator.hh"

#include "G4Types.hh"

/**
 * @brief Sinusoidal modulator as a function of global time.
 * 
 * factor = offset + scale * sin(2pi*frequency*T + phase)
 * 
 * @author Fabian Metzger
 */

class BDSModulatorSinT: public BDSModulator
{
public:
  BDSModulatorSinT() = delete;
  explicit BDSModulatorSinT(G4double frequencyIn,
                            G4double phaseIn            = 0,
                            G4double synchronousTIn     = 0,
                            G4double amplitudeOffsetIn  = 0,
                            G4double amplitudeScaleIn   = 1.0);
  virtual ~BDSModulatorSinT() = default;

  /// Returns value of equation in brief at the top of this file.
  virtual G4double Factor(const G4ThreeVector& xyz,
                          G4double T) const;
  
  virtual G4bool VariesWithTime() const {return true;}
  
  /// Return the wavelength / 20 of the oscillator.
  virtual G4double RecommendedMaxStepLength() const;

private:
  G4double angularFrequency;
  G4double phase;
  G4double synchronousT;
  G4double offset;
  G4double scale;
};

#endif

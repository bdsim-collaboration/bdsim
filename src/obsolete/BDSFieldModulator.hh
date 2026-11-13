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
#ifndef BDSFIELDMODULATOR_H
#define BDSFIELDMODULATOR_H

#include "globals.hh" // geant4 types / globals

/**
 * @brief Class to modulate any given fieldmap (electric, magnetic or electro-magnetic) 
 * by a time-dependent sin/cos.
 * 
 * This implements the GetValue method that gives back the value of sin/cos
 * for the time the particle arrives at the element of interest.
 * 
 * @author Fabian Metzger
 */

class BDSFieldModulator
{
    public:
        BDSFieldModulator(const G4double f = 0, const G4double tOff = 0, const G4double phi = 0, const G4String modulation = "");
        ~BDSFieldModulator();
        G4double GetValue(G4double t) const;
        G4double GetFrequency() const {return BDSFieldModulator::frequency;}
        G4double GetPhase() const {return BDSFieldModulator::phase;}

    private:
        G4double frequency;
        G4double tOffset;
        G4double phase;
        G4String modulator;
};

#endif
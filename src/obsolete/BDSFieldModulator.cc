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
#include "BDSFieldModulator.hh"
#include "BDSWarning.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#include "CLHEP/Units/SystemOfUnits.h"

BDSFieldModulator::BDSFieldModulator(const G4double f, const G4double tOff, const G4double phi, const G4String modulation)
{
    BDSFieldModulator::frequency = f;
    BDSFieldModulator::modulator = modulation;

    if(BDS::IsFinite(tOff))
    {
        BDSFieldModulator::phase = 2.0 * CLHEP::pi * BDSFieldModulator::frequency * tOff;
    }
    else
    {
        BDSFieldModulator::phase = phi;
    }
}

BDSFieldModulator::~BDSFieldModulator()
{

}

G4double BDSFieldModulator::GetValue(G4double t) const
{   
    if((BDSFieldModulator::modulator.compare("sin") == 0) or (BDSFieldModulator::modulator.compare("SIN") == 0) or BDSFieldModulator::modulator.compare("Sin") == 0)
    {
        return std::sin(2.0 * CLHEP::pi * BDSFieldModulator::frequency * t - BDSFieldModulator::phase);
    }
    else if((BDSFieldModulator::modulator.compare("cos") == 0) or (BDSFieldModulator::modulator.compare("COS") == 0) or (BDSFieldModulator::modulator.compare("Cos") == 0))
    {
        return std::cos(2.0 * CLHEP::pi * BDSFieldModulator::frequency * t - BDSFieldModulator::phase);
    }
    else
    {
        BDS::Warning("Unknown modulation chosen. No modulation applied meaning multiplication by const 1!");
        return 1.0;
    }
}
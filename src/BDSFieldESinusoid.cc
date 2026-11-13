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
#include "BDSFieldESinusoid.hh"
#include "BDSMagnetStrength.hh"
#include "BDSUtilities.hh"

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

BDSFieldESinusoid::BDSFieldESinusoid(BDSMagnetStrength const* strength):
  BDSFieldESinusoid((*strength)["efield"],
                    G4ThreeVector( (*strength)["ex"], (*strength)["ey"], (*strength)["ez"]),
                    (*strength)["frequency"],
                    (*strength)["phase"],
                    (*strength)["synchronousT0"])
{;}

BDSFieldESinusoid::BDSFieldESinusoid(G4double eFieldAmplitudeIn,
                                     const G4ThreeVector& unitDirectionIn,
                                     G4double frequencyIn,
                                     G4double phaseIn,
                                     G4double synchronousTIn):
  eField(eFieldAmplitudeIn),
  unitDirection(unitDirectionIn.unit()),
  angularFrequency(CLHEP::twopi*frequencyIn),
  phase(phaseIn),
  synchronousT(synchronousTIn)
{
  finiteStrength = BDS::IsFinite(eField);
}

G4ThreeVector BDSFieldESinusoid::GetField(const G4ThreeVector& /*position*/,
                                          const G4double       t) const
{
  G4double ef = eField * std::cos(angularFrequency*(t - synchronousT) + phase);
  G4ThreeVector field = unitDirection * ef;
  return field;
}

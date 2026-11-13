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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSModulatorSinT.hh"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <limits>

BDSModulatorSinT::BDSModulatorSinT(G4double frequencyIn,
                                   G4double phaseIn,
                                   G4double synchronousTIn,
                                   G4double amplitudeOffsetIn,
                                   G4double amplitudeScaleIn):
  angularFrequency(CLHEP::twopi * frequencyIn),
  phase(phaseIn),
  synchronousT(synchronousTIn),
  offset(amplitudeOffsetIn),
  scale(amplitudeScaleIn)
{
  if (frequencyIn < 0)
    {throw BDSException(__METHOD_NAME__, "\frequency\" must be >= 0");}
}

G4double BDSModulatorSinT::Factor(const G4ThreeVector& /*xyz*/,
                                  G4double T) const
{
  G4double factor = offset + scale*std::sin(angularFrequency*(T-synchronousT) + phase);
  return factor;
}

G4double BDSModulatorSinT::RecommendedMaxStepLength() const
{
  if (angularFrequency == 0)
    {return std::numeric_limits<double>::max();}
  else
    {
      G4double wavelength = CLHEP::twopi * CLHEP::c_light / angularFrequency;
      return wavelength / 20;
    }
}

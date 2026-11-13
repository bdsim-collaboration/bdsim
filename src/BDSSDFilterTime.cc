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
#include "BDSSDFilterTime.hh"

#include "G4Step.hh"
#include "G4String.hh"
#include "G4Types.hh"

BDSSDFilterTime::BDSSDFilterTime(const G4String& name,
				 G4double lowerLimitIn,
				 G4double upperLimitIn):
  G4VSDFilter(name),
  lowerLimit(lowerLimitIn),
  upperLimit(upperLimitIn)
{;}

G4bool BDSSDFilterTime::Accept(const G4Step* aStep) const
{
  G4double time = aStep->GetTrack()->GetGlobalTime();
  if (time < lowerLimit)
    {return false;}
  if (time >= upperLimit)
    {return false;}
  return true;
}

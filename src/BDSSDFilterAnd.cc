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
#include "BDSSDFilterAnd.hh"

#include "G4Step.hh"
#include "G4String.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VSDFilter.hh"

#include <vector>

BDSSDFilterAnd::BDSSDFilterAnd(const G4String& name,
			       G4bool ownsTheFiltersIn):
  G4VSDFilter(name),
  ownsTheFilters(ownsTheFiltersIn)
{;}

BDSSDFilterAnd::~BDSSDFilterAnd()
{
  if (ownsTheFilters)
    {
      for (auto filter : filters)
	{delete filter;}
    }
}

G4bool BDSSDFilterAnd::Accept(const G4Step* step) const
{
  G4bool result = true;
  for (const auto filter : filters)
    {result &= filter->Accept(step);}
  return result;
}

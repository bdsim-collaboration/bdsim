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
#include "BDSSensitiveDetector.hh"

#include "G4VSDFilter.hh"

class G4TouchableHistory;

BDSSensitiveDetector::BDSSensitiveDetector(const G4String& name):
  G4VSensitiveDetector(name)
{;}

G4bool BDSSensitiveDetector::ProcessHitsOrdered(G4Step* step,
						G4TouchableHistory* rOHist,
						const std::vector<G4VHit*>& /*hits*/)
{
  return ProcessHits(step, rOHist);
}

G4bool BDSSensitiveDetector::HitOrdered(G4Step* step,
					const std::vector<G4VHit*>& hits)
{
  if (!isActive())
    {return false;}
  
  if (filter)
    {
      if (!(filter->Accept(step)))
	{return false;}
    }
  
  if (ROgeometry)
    {
      G4TouchableHistory* rOHist = nullptr;
      if (!(ROgeometry->CheckROVolume(step, rOHist)))
	{return false;}
    }
  
  return ProcessHitsOrdered(step, nullptr, hits);
}
